cmake_minimum_required(VERSION 3.10)
project(test_speed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(test_speed test_Speed.cpp)

# Link against the core raytracer library
target_link_libraries(test_speed PRIVATE raytracer_lib)

target_compile_definitions(test_speed PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_data"
)
