# Description:
#   RPC communication interfaces and implementations for TensorFlow.

load("//tensorflow/core/platform:rules_cc.bzl", "cc_library")
load(
    "//tensorflow:tensorflow.bzl",
    "if_windows",
    "tf_cc_binary",
    "tf_cc_test",
    "tf_cuda_library",
)
load("//tensorflow:tensorflow.bzl", "tf_cuda_cc_test")  # buildifier: disable=same-origin-load
load("//tensorflow:tensorflow.bzl", "tf_cuda_cc_tests")  # buildifier: disable=same-origin-load
load("//tensorflow:tensorflow.bzl", "tf_grpc_dependencies")  # buildifier: disable=same-origin-load
load("//tensorflow:tensorflow.bzl", "tf_grpc_cc_dependencies")  # buildifier: disable=same-origin-load
load("//tensorflow:tensorflow.bzl", "filegroup")  # buildifier: disable=same-origin-load

# For platform specific build config
load(
    "//tensorflow/core/platform:build_config.bzl",
    "tf_kernel_tests_linkstatic",
    "tf_protos_profiler_service",
)
load(
    "//tensorflow/core/platform:build_config_root.bzl",
    "tf_cuda_tests_tags",
)

package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],
)

filegroup(
    name = "c_srcs",
    data = glob([
        "**/*.cc",
        "**/*.h",
    ]),
)

cc_library(
    name = "grpc_util",
    srcs = ["grpc_util.cc"],
    hdrs = ["grpc_util.h"],
    linkopts = if_windows(["-DEFAULTLIB:ws2_32.lib"]),
    deps = [
        "//tensorflow/core/distributed_runtime:error_payloads",
        "//tensorflow/core/protobuf:for_core_protos_cc",
        "//tensorflow/core:lib",
        # Required to be able to overload TensorResponse parsing.
        "//tensorflow/core/distributed_runtime:tensor_coding",
        "//tensorflow/core:lib_internal",
    ] + tf_grpc_dependencies() + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_client_cq_tag",
    srcs = [],
    hdrs = ["grpc_client_cq_tag.h"],
    deps = [
        ":grpc_util",
        "//tensorflow/core:lib",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_state",
    srcs = ["grpc_state.cc"],
    hdrs = ["grpc_state.h"],
    deps = [
        ":grpc_client_cq_tag",
        ":grpc_util",
        "@com_google_absl//absl/strings:str_format",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core/distributed_runtime:call_options",
        "//tensorflow/core/distributed_runtime:tensor_coding",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_remote_worker",
    srcs = ["grpc_remote_worker.cc"],
    hdrs = ["grpc_remote_worker.h"],
    deps = [
        ":grpc_client_cq_tag",
        ":grpc_state",
        ":grpc_util",
        ":grpc_worker_service_impl",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core/distributed_runtime:call_options",
        "//tensorflow/core/distributed_runtime:tensor_coding",
        "//tensorflow/core/distributed_runtime:worker_cache_logger",
        "//tensorflow/core/distributed_runtime:worker_interface",
        "//tensorflow/core/protobuf:worker_proto_cc",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_channel_common",
    hdrs = ["grpc_channel_common.h"],
    deps = [
        ":grpc_util",
        "//tensorflow/core:framework_lite",
        "//tensorflow/core:tflite_portable_logging",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_library(
    name = "grpc_channel",
    srcs = ["grpc_channel.cc"],
    hdrs = ["grpc_channel.h"],
    deps = [
        ":grpc_channel_common",
        ":grpc_util",
        "@com_google_absl//absl/strings",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core:protos_all_cc",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_tensor_coding",
    srcs = ["grpc_tensor_coding.cc"],
    hdrs = ["grpc_tensor_coding.h"],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core/protobuf:worker_proto_cc",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_call",
    srcs = [],
    hdrs = ["grpc_call.h"],
    deps = [
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "async_service_interface",
    srcs = [],
    hdrs = ["async_service_interface.h"],
    deps = [],
)

cc_library(
    name = "grpc_worker_cache",
    srcs = ["grpc_worker_cache.cc"],
    hdrs = ["grpc_worker_cache.h"],
    deps = [
        ":grpc_channel",
        ":grpc_client_cq_tag",
        ":grpc_remote_worker",
        ":grpc_util",
        "//tensorflow/core:lib",
        "//tensorflow/core/distributed_runtime:worker_cache",
        "//tensorflow/core/distributed_runtime:worker_cache_logger",
        "//tensorflow/core/distributed_runtime:worker_cache_partial",
        "//tensorflow/core/distributed_runtime:worker_interface",
        "//tensorflow/core/distributed_runtime/rpc/coordination:grpc_coordination_client",
        "//tensorflow/core/distributed_runtime/rpc/eager:grpc_eager_client",
        "//tensorflow/core/util:env_var",
    ],
)

tf_cc_test(
    name = "grpc_worker_cache_test",
    size = "small",
    srcs = [
        "grpc_worker_cache_test.cc",
    ],
    deps = [
        ":grpc_worker_cache",
        "//tensorflow/c:tf_status_headers",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "//tensorflow/core/distributed_runtime:test_utils",
        "//tensorflow/core/distributed_runtime/rpc:grpc_channel",
        "//tensorflow/core/platform:env",
        "//tensorflow/core/platform:status",
        "//tensorflow/core/platform:strcat",
    ],
)

cc_library(
    name = "grpc_response_cache",
    srcs = ["grpc_response_cache.cc"],
    hdrs = ["grpc_response_cache.h"],
    deps = [
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "@com_google_absl//absl/types:optional",
    ],
)

tf_cuda_library(
    name = "grpc_worker_service",
    srcs = ["grpc_worker_service.cc"],
    hdrs = ["grpc_worker_service.h"],
    deps = [
        ":async_service_interface",
        ":grpc_call",
        ":grpc_response_cache",
        ":grpc_tensor_coding",
        ":grpc_util",
        ":grpc_worker_service_impl",
        "@com_google_absl//absl/container:flat_hash_map",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core/distributed_runtime:graph_mgr",
        "//tensorflow/core/distributed_runtime:rendezvous_mgr_interface",
        "//tensorflow/core/distributed_runtime:worker",
        "//tensorflow/core/distributed_runtime:worker_cache",
        "//tensorflow/core/distributed_runtime:worker_env",
        "//tensorflow/core/distributed_runtime:worker_session",
        "//tensorflow/core/profiler/lib:scoped_memory_debug_annotation",
        "//tensorflow/core/protobuf:worker_proto_cc",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_worker_service_impl",
    srcs = ["grpc_worker_service_impl.cc"],
    hdrs = ["grpc_worker_service_impl.h"],
    deps = [
        ":grpc_util",
        "//tensorflow/core/distributed_runtime:tensor_coding",
        "//tensorflow/core/protobuf:worker_proto_cc",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "grpc_remote_master",
    srcs = ["grpc_remote_master.cc"],
    hdrs = ["grpc_remote_master.h"],
    deps = [
        ":grpc_master_service_impl",
        ":grpc_util",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core/distributed_runtime:call_options",
        "//tensorflow/core/distributed_runtime:master_interface",
        "//tensorflow/core/profiler/lib:traceme",
        "//tensorflow/core/protobuf:master_proto_cc",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "grpc_master_service",
    srcs = ["grpc_master_service.cc"],
    hdrs = ["grpc_master_service.h"],
    deps = [
        ":async_service_interface",
        ":grpc_call",
        ":grpc_master_service_impl",
        ":grpc_util",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core/distributed_runtime:master",
        "//tensorflow/core/profiler/lib:traceme",
        "//tensorflow/core/protobuf:master_proto_cc",
    ] + tf_grpc_cc_dependencies(),
    alwayslink = 1,
)

cc_library(
    name = "grpc_master_service_impl",
    srcs = ["grpc_master_service_impl.cc"],
    hdrs = ["grpc_master_service_impl.h"],
    deps = [
        "//tensorflow/core/protobuf:master_proto_cc",
    ] + tf_grpc_cc_dependencies(),
)

cc_library(
    name = "rpc_rendezvous_mgr",
    srcs = ["rpc_rendezvous_mgr.cc"],
    hdrs = ["rpc_rendezvous_mgr.h"],
    deps = [
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core/distributed_runtime:base_rendezvous_mgr",
        "//tensorflow/core/distributed_runtime:request_id",
        "//tensorflow/core/distributed_runtime:tensor_coding",
        "//tensorflow/core/distributed_runtime:worker_cache",
        "//tensorflow/core/distributed_runtime:worker_env",
        "//tensorflow/core/distributed_runtime:worker_interface",
    ],
)

cc_library(
    name = "grpc_server_lib",
    srcs = ["grpc_server_lib.cc"],
    hdrs = ["grpc_server_lib.h"],
    linkstatic = 1,  # Seems to be needed since alwayslink is broken in bazel
    deps = [
        ":async_service_interface",
        ":grpc_channel",
        ":grpc_master_service",
        ":grpc_worker_cache",
        ":grpc_worker_service",
        ":rpc_rendezvous_mgr",
        "//tensorflow/core/nccl:collective_communicator",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core/common_runtime/eager:context",
        "//tensorflow/core/distributed_runtime:collective_param_resolver_distributed",
        "//tensorflow/core/distributed_runtime:device_resolver_distributed",
        "//tensorflow/core/distributed_runtime:graph_mgr",
        "//tensorflow/core/distributed_runtime:local_master",
        "//tensorflow/core/distributed_runtime:master",
        "//tensorflow/core/distributed_runtime:master_env",
        "//tensorflow/core/distributed_runtime:master_session",
        "//tensorflow/core/distributed_runtime:rpc_collective_executor_mgr",
        "//tensorflow/core/distributed_runtime:server_lib",
        "//tensorflow/core/distributed_runtime:session_mgr",
        "//tensorflow/core/distributed_runtime:worker_cache_wrapper",
        "//tensorflow/core/distributed_runtime:worker_env",
        "//tensorflow/core/distributed_runtime/rpc/eager:grpc_eager_service_impl",
        "//tensorflow/core/profiler/rpc:profiler_service_impl",
    ] + tf_protos_profiler_service() + tf_grpc_dependencies() + tf_grpc_cc_dependencies(),
    alwayslink = 1,
)

cc_library(
    name = "grpc_runtime",
    visibility = ["//visibility:public"],
    deps = [
        ":grpc_server_lib",
        ":grpc_session",
    ],
)

tf_cc_binary(
    name = "grpc_tensorflow_server",
    srcs = [
        "grpc_tensorflow_server.cc",
    ],
    deps = [
        ":grpc_server_lib",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:data_flow_ops_op_lib",
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:functional_ops_op_lib",
        "//tensorflow/core:lib",
        "//tensorflow/core:lookup_ops_op_lib",
        "//tensorflow/core:math_ops_op_lib",
        "//tensorflow/core:no_op_op_lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:sendrecv_ops_op_lib",
        "//tensorflow/core/distributed_runtime:server_lib",
        "//tensorflow/core/kernels:data_flow",
    ] + tf_grpc_cc_dependencies(),
)

# Library version of grpc_testlib_server, to allow for custom testlib servers.
cc_library(
    name = "grpc_testlib_server_main",
    testonly = 1,
    srcs = [
        "grpc_testlib_server.cc",
    ],
    deps = [
        ":grpc_server_lib",
        "//tensorflow/core:array_ops_op_lib",
        "//tensorflow/core:bitwise_ops_op_lib",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:lib",
        "//tensorflow/core:nn_ops_op_lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:state_ops_op_lib",
        "//tensorflow/core:testlib",
        "//tensorflow/core/distributed_runtime:server_lib",
        "//tensorflow/core/kernels:constant_op",
        "//tensorflow/core/kernels:cwise_op",
        "//tensorflow/core/kernels:dense_update_ops",
        "//tensorflow/core/kernels:identity_op",
        "//tensorflow/core/kernels:matmul_op",
        "//tensorflow/core/kernels:reduction_ops",
        "//tensorflow/core/kernels:variable_ops",
    ] + tf_grpc_cc_dependencies(),
    alwayslink = 1,
)

tf_cc_binary(
    name = "grpc_testlib_server",
    testonly = 1,
    deps = [":grpc_testlib_server_main"],
)

tf_cuda_library(
    name = "grpc_testlib",
    testonly = 1,
    srcs = ["grpc_testlib.cc"],
    hdrs = ["grpc_testlib.h"],
    data = [
        ":grpc_testlib_server",
    ],
    visibility = ["//tensorflow:internal"],
    deps = [
        ":grpc_session",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:test",
        "//tensorflow/core:testlib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "grpc_session",
    srcs = ["grpc_session.cc"],
    hdrs = ["grpc_session.h"],
    deps = [
        ":grpc_channel",
        ":grpc_remote_master",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core/distributed_runtime:call_options",
        "//tensorflow/core/distributed_runtime:local_master",
        "//tensorflow/core/distributed_runtime:master_interface",
        "//tensorflow/core/distributed_runtime:message_wrappers",
        "//tensorflow/core/distributed_runtime:request_id",
        "//tensorflow/core/protobuf:master_proto_cc",
    ],
    alwayslink = 1,
)

tf_cuda_cc_tests(
    name = "rpc_tests",
    size = "small",
    srcs = [
        "grpc_channel_test.cc",
        "rpc_rendezvous_mgr_test.cc",
    ],
    linkopts = select({
        "//tensorflow:macos": ["-headerpad_max_install_names"],
        "//conditions:default": [],
    }),
    linkstatic = tf_kernel_tests_linkstatic(),
    tags = tf_cuda_tests_tags() + [],
    deps = [
        ":grpc_channel",
        ":grpc_server_lib",
        ":grpc_session",
        ":grpc_testlib",
        ":rpc_rendezvous_mgr",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "//tensorflow/core:testlib",
        "//tensorflow/core/distributed_runtime:server_lib",
        "//tensorflow/core/distributed_runtime:test_utils",
        "//tensorflow/core/platform:blocking_counter",
        "//tensorflow/core/protobuf:master_proto_cc",
    ],
)

tf_cc_test(
    name = "grpc_tensor_coding_test",
    size = "small",
    srcs = ["grpc_tensor_coding_test.cc"],
    tags = [
        "no_mac",
        "no_windows",
    ],
    deps = [
        ":grpc_tensor_coding",
        ":grpc_testlib",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "//tensorflow/core:testlib",
        "//tensorflow/core/protobuf:worker_proto_cc",
    ] + tf_grpc_cc_dependencies(),
)

tf_cc_test(
    name = "grpc_util_test",
    size = "small",
    srcs = ["grpc_util_test.cc"],
    tags = [
        "no_mac",
    ],
    deps = [
        ":grpc_util",
        "//tensorflow/core/distributed_runtime:error_payloads",
        "//tensorflow/core/platform:errors",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "//tensorflow/core/protobuf:worker_proto_cc",
    ] + tf_grpc_dependencies() + tf_grpc_cc_dependencies(),
)

tf_cuda_cc_test(
    name = "grpc_session_test",
    size = "medium",
    srcs = ["grpc_session_test.cc"],
    linkstatic = tf_kernel_tests_linkstatic(),
    tags = tf_cuda_tests_tags() + [
        "no_cuda_asan",  # b/176448181
        "no_oss",  # b/62956105: port conflicts.
    ],
    deps = [
        ":grpc_channel",
        ":grpc_server_lib",
        ":grpc_session",
        ":grpc_testlib",
        ":rpc_rendezvous_mgr",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:state_ops_op_lib",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "//tensorflow/core:testlib",
        "//tensorflow/core/distributed_runtime:server_lib",
        "//tensorflow/core/kernels:constant_op",
        "//tensorflow/core/kernels:dense_update_ops",
        "//tensorflow/core/kernels:matmul_op",
        "//tensorflow/core/kernels:variable_ops",
        "//tensorflow/core/protobuf:master_proto_cc",
    ],
)
