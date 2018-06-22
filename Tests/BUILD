cc_test(
    name = "Tests",
    srcs = glob(["*.cpp"]),
    linkopts = [
        "-lgtest",
        "-lgtest_main",
        "-lpthread",
    ],
    deps = [
        "//Lib:Lib",
    ],
)