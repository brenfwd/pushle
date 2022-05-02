cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(language)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "-O3 -g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g -Wall -Wextra")

add_executable(assembler src/assembler.cpp)
add_executable(vm src/main.cpp src/vm.cpp)
target_include_directories(assembler PUBLIC "${PROJECT_BINARY_DIR}/include")
target_include_directories(vm PUBLIC "${PROJECT_BINARY_DIR}/include")

find_package(fmt CONFIG REQUIRED)
target_link_libraries(assembler fmt::fmt-header-only)
target_link_libraries(vm fmt::fmt-header-only)
