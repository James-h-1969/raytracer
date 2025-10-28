cmake_minimum_required(VERSION 3.16)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# --- Test executable ---
add_executable(run_tests
    test_Image.cpp
)

# Link against GoogleTest AND your main library
target_link_libraries(run_tests
    PRIVATE
        raytracer_lib     # <-- link your project’s library
        GTest::gtest_main
        GTest::gtest
        GTest::gmock
)

# Inherit include directories from the project
target_include_directories(run_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(run_tests PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_data"
)

# --- Enable test discovery ---
include(GoogleTest)
gtest_discover_tests(run_tests)
