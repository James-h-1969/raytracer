cmake_minimum_required(VERSION 3.16)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(runTests
    test_Image.cpp
)

target_link_libraries(runTests
    PRIVATE
        GTest::gtest_main
        GTest::gtest
        GTest::gmock
        raytracer  
)

include(GoogleTest)
gtest_discover_tests(runTests)
