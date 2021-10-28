workspace(name = "com_github_kleinpa_homedmx")

load("@com_github_kleinpa_homedmx//:repos.bzl", "homedmx_repos")

homedmx_repos()

load("@com_github_kleinpa_homedmx//:deps.bzl", "homedmx_deps")

homedmx_deps()

load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories",
)

_cc_image_repos()
