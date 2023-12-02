load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
#load("@prometheus_cpp//bazel:repositories.bzl", "prometheus_cpp_repositories")

#def homedmx_deps():
#prometheus_cpp_repositories()

def _repos_module_impl(module_ctx):
    http_archive(
        build_file = "@dmxhue//external:etcpal-BUILD.bazel",
        name = "etcpal",
        sha256 = "e94d0f492a142ee998acf64e84c1cd17cefb962c0ac389a8083232ad13f2c77b",
        strip_prefix = "EtcPal-eb5f906aafdbc6a4cd3e00670bbd2824e34705cd",
        urls = ["https://github.com/ETCLabs/etcpal/archive/eb5f906aafdbc6a4cd3e00670bbd2824e34705cd.zip"],
    )
    http_archive(
        build_file = "@dmxhue//external:sacn-BUILD.bazel",
        name = "sacn",
        sha256 = "baee0ec371bd435f85e5717f8a6762270202872e77ffc788b5692c3110d53196",
        strip_prefix = "sACN-af246e04106f2342937566097322ac1ebb86fae0",
        urls = ["https://github.com/ETCLabs/sACN/archive/af246e04106f2342937566097322ac1ebb86fae0.zip"],
    )

    http_archive(
        build_file = "@dmxhue//external:hueplusplus-BUILD.bazel",
        name = "hueplusplus",
        sha256 = "",
        strip_prefix = "hueplusplus-c2b94d0f23d6f535db4ec943d15a97979ab72234",
        urls = ["https://github.com/kleinpa/hueplusplus/archive/c2b94d0f23d6f535db4ec943d15a97979ab72234.zip"],
    )
    http_archive(
        name = "mbedtls",
        build_file = "@dmxhue//external:mbedtls-BUILD.bazel",
        sha256 = "e580970a766fdaba2b70bcf3e69de34a67684e7f23a815b97f79771382c43651",
        strip_prefix = "mbedtls-2.27.0",
        urls = [
            "https://github.com/ARMmbed/mbedtls/archive/v2.27.0.zip",
        ],
    )

    # http_archive(
    #     name = "util_linux",
    #     build_file = "@dmxhue//external:util-linux-BUILD.bazel",
    #     sha256 = "faa8b46d080faa6f32c57da81eda871e38e1e27ba4e9b61cb2589334671aba50",
    #     strip_prefix = "util-linux-2.37",
    #     urls = ["https://mirrors.edge.kernel.org/pub/linux/utils/util-linux/v2.37/util-linux-2.37.tar.gz"],
    # )

    # http_archive(
    #     name = "gflags",
    #     sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    #     strip_prefix = "gflags-2.2.2",
    #     urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
    # )

repos_bzlmod = module_extension(implementation = _repos_module_impl)
