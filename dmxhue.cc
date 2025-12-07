
#include <hueplusplus/Bridge.h>
#include <hueplusplus/EntertainmentMode.h>
#include <hueplusplus/LinHttpHandler.h>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/marshalling.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "etcpal/cpp/inet.h"
#include "hueplusplus/Bridge.h"
#include "hueplusplus/EntertainmentMode.h"
#include "hueplusplus/Group.h"
#include "hueplusplus/HueException.h"
#include "hueplusplus/Light.h"
#include "hueplusplus/StateTransaction.h"
#include "sacn/common.h"
#include "sacn/cpp/common.h"
#include "sacn/cpp/merge_receiver.h"
/**
 * dmxhue is a service that listens to UDP sACN data and sends it to
 * use Philips Hue API using Entertainment Mode. Entertainment Mode
 * supports a high refresh rate but only works for colored
 * Hue-brand lights.
 */

ABSL_FLAG(std::string, hue_ip, "", "Address of Hue bridge");
ABSL_FLAG(std::string, hue_user, "", "Hue bridge user");
ABSL_FLAG(std::string, hue_clientkey, "",
          "Hue bridge client key, used for entertainment mode");

ABSL_FLAG(int, dmx_universe, 1, "DMX universe");
ABSL_FLAG(int, dmx_address_start, 1, "First DMX address to use");
ABSL_FLAG(int, dmx_address_end, DMX_ADDRESS_COUNT, "Last DMX address to use");

using namespace std::literals;

constexpr char kGroupName[] = "dmxhue-group";
constexpr uint8_t kControlModeDisabled = 0;
constexpr uint8_t kControlModeWhite = 1;
constexpr uint8_t kControlModeColor = 2;
constexpr uint8_t kControlModeEntertainment = 3;

class SequentialAllocator {
 public:
  SequentialAllocator(int start, int end) : next_(start), end_(end) {}

  int next() {
    if (next_ >= end_) {
      throw std::runtime_error("unable to allocate");
    }
    return next_++;
  }

 private:
  int next_;
  const int end_;
};

struct DmxMapping {
  hueplusplus::Light light;
  uint8_t ch0;
  uint8_t ch1;
  uint8_t ch2 = 0;
  uint8_t ch3 = 0;
};

class EntertainmentGroupManager {
 public:
  static EntertainmentGroupManager Make(
      std::shared_ptr<hueplusplus::Bridge> bridge,
      const std::vector<int>& light_ids) {
    // Delete previously created groups with our name
    for (auto& group : bridge->groups().getAll()) {
      if (group.getName() == kGroupName) {
        LOG(INFO) << "removing group " << group.getId() << "/"
                  << group.getType() << ": " << group.getName();
        bridge->groups().remove(group.getId());
      }
    }

    LOG(INFO) << "creating group " << kGroupName << " with " << light_ids.size()
              << " lights";
    const int group_id = bridge->groups().create(
        hueplusplus::CreateGroup::Entertainment(light_ids, kGroupName));

    if (group_id == 0) {
      throw std::runtime_error("unable to create entertainment group");
    }

    std::map<int, int> light_index;
    hueplusplus::Group group = bridge->groups().get(group_id);
    int i = 0;
    for (const auto& light_id : group.getLightIds()) {
      light_index.insert({light_id, i++});
    }

    auto em = std::make_unique<hueplusplus::EntertainmentMode>(*bridge, group);

    return EntertainmentGroupManager(std::move(em), light_index);
  }
  ~EntertainmentGroupManager() {
    LOG(INFO) << "disconnecting from entertainment group";
    if (light_index_.size() > 0) {
      // em_->disconnect();
    }
  }

  void set(uint8_t light_id, uint8_t r, uint8_t g, uint8_t b) {
    if (!em_->setColorRGB(light_index_.at(light_id), r, g, b)) {
      throw std::runtime_error("unable to set color");
    }
  }
  void update() {
    if (light_index_.size() > 0) {
      if (!em_->update()) {
        // throw std::runtime_error("unable to update");
        LOG(WARNING) << "updated failed";
      } else {
        // LOG(INFO) << "updated";
      }
    }
  }

  std::vector<int> lightIDs() {
    std::vector<int> x;
    for (const auto& [k, v] : light_index_) {
      x.push_back(k);
    }
    return x;
  }

 private:
  std::shared_ptr<hueplusplus::EntertainmentMode> em_;
  const std::map<int, int> light_index_;

  EntertainmentGroupManager(  // std::shared_ptr<hueplusplus::Bridge> bridge,
      std::unique_ptr<hueplusplus::EntertainmentMode> em,
      const std::map<int, int> light_index)
      :  // bridge_(bridge),
        em_(std::move(em)),
        light_index_(light_index) {
    if (light_index_.size() > 0) {
      // Enable entertainment mode
      LOG(INFO) << "connecting to entertainment group";
      if (!em_->connect()) {
        throw std::runtime_error("unable to connect to entertainment group");
      }
    }
  }
};

// CallbackNotifyHandler wraps a std::function and calls it whenever new merged
// DMX values are available.
class CallbackNotifyHandler : public sacn::MergeReceiver::NotifyHandler {
 public:
  using CallbackFn = std::function<void(const SacnRecvMergedData&)>;
  using Handle = sacn::MergeReceiver::Handle;

  CallbackNotifyHandler(CallbackFn cb) : cb_(cb) {}

  void HandleMergedData(Handle handle,
                        const SacnRecvMergedData& merged_data) override {
    ++data_count_;
    if (cb_) {
      cb_(merged_data);
    }
  }
  void HandleNonDmxData(Handle receiver_handle,
                        const etcpal::SockAddr& source_addr,
                        const SacnRemoteSource& source_info,
                        const SacnRecvUniverseData& universe_data) override {
    ++non_dmx_data_count_;
  };

  void HandleSourceLimitExceeded(Handle handle, uint16_t universe) override {};

 private:
  int data_count_ = 0;
  int non_dmx_data_count_ = 0;
  const CallbackFn cb_;
};

void updateLightFromDmx(DmxMapping mapping,
                        std::array<uint8_t, DMX_ADDRESS_COUNT>& dmx_data) {
  uint8_t transition = 0;
  const uint8_t mode = dmx_data[mapping.ch0];

  if (mode == kControlModeWhite) {
    uint8_t brightness = dmx_data[mapping.ch1];
    unsigned int mired = 153 + (dmx_data[mapping.ch2] / 255.0) * (500 - 153);

    if (brightness > 0) {
      if (mapping.light.hasTemperatureControl()) {
        if (!mapping.light.transaction()
                 .setOn(true)
                 .setBrightness(brightness)
                 .setColorTemperature(mired)
                 .setTransition(transition)
                 .commit()) {
          throw std::runtime_error("unable to set color temperature");
        }
      } else {
        if (!mapping.light.transaction()
                 .setOn(true)
                 .setBrightness(brightness)
                 .setTransition(transition)
                 .commit()) {
          throw std::runtime_error("unable to set color temperature");
        }
      }
    } else {
      if (!mapping.light.off(transition)) {
        throw std::runtime_error("unable to turn lights on");
      }
    }

  } else if (mode == kControlModeColor) {
    auto color = hueplusplus::RGB({dmx_data[mapping.ch1], dmx_data[mapping.ch2],
                                   dmx_data[mapping.ch3]})
                     .toXY();
    if (color.brightness > 0 && mapping.light.hasColorControl()) {
      if (!mapping.light.transaction()
               .setOn(true)
               .setColor(color)
               .setTransition(transition)
               .commit()) {
        throw std::runtime_error("unable to set color temperature");
      }

    } else {
      if (!mapping.light.off(transition)) {
        throw std::runtime_error("unable to turn lights off");
      }
    }
  } else if (mode == kControlModeEntertainment) {
    if (!mapping.light.off(transition)) {
      throw std::runtime_error("unable to turn lights off");
    }
  } else if (mode == kControlModeDisabled) {
  } else {
    // throw std::runtime_error("unknown mode");
  }
}

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  // Configure metrics
  prometheus::Exposer exposer{"0.0.0.0:5569"};
  auto registry = std::make_shared<prometheus::Registry>();
  auto& sacn_dmx_data_packet_family = prometheus::BuildCounter()
                                          .Name("sacn_dmx_data_packet")
                                          .Help("Number of observed packets")
                                          .Register(*registry);
  auto& sacn_dmx_data_packet_counter = sacn_dmx_data_packet_family.Add({});
  exposer.RegisterCollectable(registry);

  auto handler = std::make_shared<hueplusplus::LinHttpHandler>();
  auto bridge = std::make_shared<hueplusplus::Bridge>(
      absl::GetFlag(FLAGS_hue_ip), 80, absl::GetFlag(FLAGS_hue_user), handler,
      absl::GetFlag(FLAGS_hue_clientkey));

  // Sort lights by ID for consistent ordering
  std::vector<hueplusplus::Light> lights = bridge->lights().getAll();
  std::sort(lights.begin(), lights.end(),
            [](hueplusplus::Light a, hueplusplus::Light b) {
              return a.getId() < b.getId();
            });

  // Allocate DMX channels depending on light capabilities
  auto allocator = SequentialAllocator(absl::GetFlag(FLAGS_dmx_address_start),
                                       absl::GetFlag(FLAGS_dmx_address_end));
  std::vector<DmxMapping> mappings;
  for (const auto& light : lights) {
    if (light.hasColorControl()) {
      mappings.push_back({light, (uint8_t)allocator.next(),
                          (uint8_t)allocator.next(), (uint8_t)allocator.next(),
                          (uint8_t)allocator.next()});
    } else if (light.hasTemperatureControl()) {
      mappings.push_back({light, (uint8_t)allocator.next(),
                          (uint8_t)allocator.next(),
                          (uint8_t)allocator.next()});
    } else {
      mappings.push_back(
          {light, (uint8_t)allocator.next(), (uint8_t)allocator.next()});
    }

    // Log configuration
    LOG(INFO) << "configured id:" << light.getId() << " name:\""
              << light.getName() << "\"";
  }

  auto egm = std::make_unique<EntertainmentGroupManager>(
      EntertainmentGroupManager::Make(bridge, {}));

  std::array<uint8_t, DMX_ADDRESS_COUNT> dmx_data;
  std::mutex dmx_data_mutex;

  // Prepare sACN callback
  CallbackNotifyHandler sacn_handler([&sacn_dmx_data_packet_counter, &dmx_data,
                                      &dmx_data_mutex, &mappings,
                                      &egm](const SacnRecvMergedData& data) {
    sacn_dmx_data_packet_counter.Increment();

    {
      std::lock_guard<std::mutex> lock(dmx_data_mutex);
      for (int i = 0; i < data.slot_range.address_count; i++) {
        dmx_data[data.slot_range.start_address + i] = data.slots[i];
      }
    }

    if (!egm) return;

    for (const int light_id : egm->lightIDs()) {
      for (const auto& mapping : mappings) {
        if (mapping.light.getId() == light_id) {
          uint8_t r = dmx_data[mapping.ch1];
          uint8_t g = dmx_data[mapping.ch2];
          uint8_t b = dmx_data[mapping.ch3];
          egm->set(light_id, r, g, b);
        }
      }
    }
    egm->update();
  });

  // Start sACN receiver
  sacn::Init();
  sacn::MergeReceiver::Settings sacn_config(absl::GetFlag(FLAGS_dmx_universe));
  sacn::MergeReceiver merge_receiver;

  LOG(INFO) << "starting sACN handler";
  merge_receiver.Startup(sacn_config, sacn_handler);

  auto next = std::chrono::system_clock::now();
  auto frame_rate = 3;  // 1-44hz?
  auto frame_time = std::chrono::milliseconds(1000) / frame_rate;

  while (true) {
    std::this_thread::sleep_until(next += frame_time);

    std::vector<int> em_lights;
    for (const auto& mapping : mappings) {
      if (mapping.light.hasColorControl() &&
          dmx_data[mapping.ch0] == kControlModeEntertainment) {
        em_lights.push_back(mapping.light.getId());
      }

      try {
        updateLightFromDmx(mapping, dmx_data);
      } catch (const hueplusplus::HueAPIResponseException& e) {
        LOG(ERROR) << "hue api error updating: " << mapping.light.getName()
                   << ": " << e.GetDescription();
      } catch (const std::runtime_error& e) {
        LOG(ERROR) << "error updating light " << mapping.light.getName() << ": "
                   << e.what();
      }
    }

    if (egm->lightIDs() != em_lights) {
      egm = std::make_unique<EntertainmentGroupManager>(
          EntertainmentGroupManager::Make(bridge, em_lights));
    }

    // Entertainment mode requires updates at least once per second.
    // TODO: use concurrency construct here
    egm->update();
  }
  LOG(INFO) << "stopping";
  merge_receiver.Shutdown();
  sacn::Deinit();
}
