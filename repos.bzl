load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def homedmx_repos():
    if not native.existing_rule("com_google_protobuf"):
        http_archive(
            name = "com_google_protobuf",
            sha256 = "bf0e5070b4b99240183b29df78155eee335885e53a8af8683964579c214ad301",
            strip_prefix = "protobuf-3.14.0",
            url = "https://github.com/protocolbuffers/protobuf/archive/v3.14.0.zip",
        )
    if not native.existing_rule("com_github_etclabs_etcpal"):
        http_archive(
            build_file = "@com_github_kleinpa_homedmx//external:etcpal-BUILD.bazel",
            name = "com_github_etclabs_etcpal",
            sha256 = "e94d0f492a142ee998acf64e84c1cd17cefb962c0ac389a8083232ad13f2c77b",
            strip_prefix = "EtcPal-eb5f906aafdbc6a4cd3e00670bbd2824e34705cd",
            urls = ["https://github.com/ETCLabs/etcpal/archive/eb5f906aafdbc6a4cd3e00670bbd2824e34705cd.zip"],
        )
    if not native.existing_rule("com_github_etclabs_sacn"):
        http_archive(
            build_file = "@com_github_kleinpa_homedmx//external:sacn-BUILD.bazel",
            name = "com_github_etclabs_sacn",
            sha256 = "baee0ec371bd435f85e5717f8a6762270202872e77ffc788b5692c3110d53196",
            strip_prefix = "sACN-af246e04106f2342937566097322ac1ebb86fae0",
            urls = ["https://github.com/ETCLabs/sACN/archive/af246e04106f2342937566097322ac1ebb86fae0.zip"],
        )
    if not native.existing_rule("com_github_nlohmann_json"):
        http_archive(
            build_file = "@com_github_kleinpa_homedmx//external:json-BUILD.bazel",
            name = "com_github_nlohmann_json",
            sha256 = "a5af2225a81b5e8842ee6a95715674e734b91b1d8d23a964dc23d7eac338ca60",
            strip_prefix = "json-f5b3fb326c1a651cd3e62201b0e7074cf987f748",
            urls = ["https://github.com/nlohmann/json/archive/f5b3fb326c1a651cd3e62201b0e7074cf987f748.zip"],
        )
    if not native.existing_rule("com_github_enwi_hueplusplus"):
        http_archive(
            build_file = "@com_github_kleinpa_homedmx//external:hueplusplus-BUILD.bazel",
            name = "com_github_enwi_hueplusplus",
            patch_cmds = ["find . -type f -exec sed -i 's/json\\/json\\.hpp/nlohmann\\/json\\.hpp/g' {} \\;"],
            sha256 = "b001a587e52ef482e9685de5103e6907891f289f724becfcf039639543db2f9b",
            strip_prefix = "hueplusplus-ee0f629864bea2e71c3b06e7bc5cbc4344141905",
            urls = ["https://github.com/kleinpa/hueplusplus/archive/ee0f629864bea2e71c3b06e7bc5cbc4344141905.zip"],
        )
    if not native.existing_rule("com_github_armmbed_mbedtls"):
        http_archive(
            name = "com_github_armmbed_mbedtls",
            build_file = "mbedtls-BUILD.bazel",
            sha256 = "e580970a766fdaba2b70bcf3e69de34a67684e7f23a815b97f79771382c43651",
            strip_prefix = "mbedtls-2.27.0",
            urls = [
                "https://github.com/ARMmbed/mbedtls/archive/v2.27.0.zip",
            ],
        )
    if not native.existing_rule("com_google_googletest"):
        http_archive(
            name = "com_google_googletest",
            sha256 = "d29b9414db0335b86d7f552daa6eda9f5aef7906c6f75b2f3f7798c710bb893a",
            strip_prefix = "googletest-075810f7a20405ea09a93f68847d6e963212fa62",
            urls = ["https://github.com/google/googletest/archive/075810f7a20405ea09a93f68847d6e963212fa62.zip"],
        )
    if not native.existing_rule("com_google_absl"):
        http_archive(
            name = "com_google_absl",
            sha256 = "bb2a0b57c92b6666e8acb00f4cbbfce6ddb87e83625fb851b0e78db581340617",
            strip_prefix = "abseil-cpp-b9b925341f9e90f5e7aa0cf23f036c29c7e454eb",
            urls = ["https://github.com/abseil/abseil-cpp/archive/b9b925341f9e90f5e7aa0cf23f036c29c7e454eb.zip"],
        )
    if not native.existing_rule("com_github_gflags_gflags"):
        http_archive(
            name = "com_github_gflags_gflags",
            sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
            strip_prefix = "gflags-2.2.2",
            urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
        )
    if not native.existing_rule("com_github_google_glog"):
        http_archive(
            name = "com_github_google_glog",
            sha256 = "d0cba6318264bc44c99dc3b932432b6c81dda4611530c2552d9e7d4c8156bd43",
            strip_prefix = "glog-9cf0eb7c98f6f611c185c5a611d6b104c612ec0e",
            urls = ["https://github.com/google/glog/archive/9cf0eb7c98f6f611c185c5a611d6b104c612ec0e.zip"],
        )
    if not native.existing_rule("com_github_jupp0r_prometheus_cpp"):
        http_archive(
            name = "com_github_jupp0r_prometheus_cpp",
            sha256 = "4b4241542a0ee807a2e351600c342b915ea02fac5ccb2235d4794812f46b50b6",
            strip_prefix = "prometheus-cpp-722fbcf18695976b23f81d1e852b8fdb4690cfd9",
            urls = ["https://github.com/jupp0r/prometheus-cpp/archive/722fbcf18695976b23f81d1e852b8fdb4690cfd9.zip"],
        )
    if not native.existing_rule("io_bazel_rules_docker"):
        http_archive(
            name = "io_bazel_rules_docker",
            sha256 = "59536e6ae64359b716ba9c46c39183403b01eabfbd57578e84398b4829ca499a",
            strip_prefix = "rules_docker-0.22.0",
            urls = ["https://github.com/bazelbuild/rules_docker/releases/download/v0.22.0/rules_docker-v0.22.0.tar.gz"],
        )
    if not native.existing_rule("org_kernel_util_linux"):
        http_archive(
            name = "org_kernel_util_linux",
            build_file = "@com_github_kleinpa_homedmx//external:util-linux-BUILD.bazel",
            sha256 = "faa8b46d080faa6f32c57da81eda871e38e1e27ba4e9b61cb2589334671aba50",
            strip_prefix = "util-linux-2.37",
            urls = ["https://mirrors.edge.kernel.org/pub/linux/utils/util-linux/v2.37/util-linux-2.37.tar.gz"],
        )
