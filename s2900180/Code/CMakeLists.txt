cmake_minimum_required(VERSION 3.16)
project(raytracer LANGUAGES CXX)

# --- Compiler setup ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Dependencies ---
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(nlohmann_json 3.2.0 REQUIRED)

# --- Include directories ---
include_directories(${PROJECT_SOURCE_DIR}/include)

# --- Source files (shared between main and tests) ---
set(SOURCES
    src/Camera.cpp
    src/Image.cpp
    src/Mesh.cpp
    src/BlenderFileReader.cpp
)

# --- Core library (shared by main and tests) ---
add_library(raytracer_lib ${SOURCES})

target_include_directories(raytracer_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(raytracer_lib
    PUBLIC Eigen3::Eigen nlohmann_json::nlohmann_json
)

target_compile_options(raytracer_lib PRIVATE
    -Wall -Wextra -Wpedantic -O2
)

# --- Main executable ---
add_executable(raytracer src/main.cpp)
target_link_libraries(raytracer PRIVATE raytracer_lib)

# --- Enable testing ---
enable_testing()
add_subdirectory(test)
