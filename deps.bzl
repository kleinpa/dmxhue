load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")
load("@com_github_jupp0r_prometheus_cpp//bazel:repositories.bzl", "prometheus_cpp_repositories")
load("@io_bazel_rules_docker//repositories:repositories.bzl", container_repositories = "repositories")

def homedmx_deps():
    protobuf_deps()
    prometheus_cpp_repositories()
    container_repositories()
