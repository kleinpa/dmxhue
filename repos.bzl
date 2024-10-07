load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def _module_impl(bzlmod = False):
    http_archive(
        build_file = "@dmxhue//external:etcpal-BUILD.bazel",
        name = "com_github_etclabs_etcpal",
        sha256 = "e94d0f492a142ee998acf64e84c1cd17cefb962c0ac389a8083232ad13f2c77b",
        strip_prefix = "EtcPal-eb5f906aafdbc6a4cd3e00670bbd2824e34705cd",
        urls = ["https://github.com/ETCLabs/etcpal/archive/eb5f906aafdbc6a4cd3e00670bbd2824e34705cd.zip"],
    )
    http_archive(
        build_file = "@dmxhue//external:sacn-BUILD.bazel",
        name = "com_github_etclabs_sacn",
        sha256 = "baee0ec371bd435f85e5717f8a6762270202872e77ffc788b5692c3110d53196",
        strip_prefix = "sACN-af246e04106f2342937566097322ac1ebb86fae0",
        urls = ["https://github.com/ETCLabs/sACN/archive/af246e04106f2342937566097322ac1ebb86fae0.zip"],
    )
    http_archive(
        build_file = "@dmxhue//external:hueplusplus-BUILD.bazel",
        name = "com_github_enwi_hueplusplus",
        patch_cmds = ["find . -type f -exec sed -i 's/json\\/json\\.hpp/nlohmann\\/json\\.hpp/g' {} \\;"],
        sha256 = "b001a587e52ef482e9685de5103e6907891f289f724becfcf039639543db2f9b",
        strip_prefix = "hueplusplus-ee0f629864bea2e71c3b06e7bc5cbc4344141905",
        urls = ["https://github.com/kleinpa/hueplusplus/archive/ee0f629864bea2e71c3b06e7bc5cbc4344141905.zip"],
    )
    http_archive(
        name = "org_kernel_util_linux",
        build_file = "@dmxhue//external:util-linux-BUILD.bazel",
        sha256 = "faa8b46d080faa6f32c57da81eda871e38e1e27ba4e9b61cb2589334671aba50",
        strip_prefix = "util-linux-2.37",
        urls = ["https://mirrors.edge.kernel.org/pub/linux/utils/util-linux/v2.37/util-linux-2.37.tar.gz"],
    )

repos_bzlmod = module_extension(implementation = _module_impl)
