cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(language)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(language src/main.cpp src/vm.cpp)
target_include_directories(language PUBLIC "${PROJECT_BINARY_DIR}/include")

find_package(fmt CONFIG REQUIRED)
target_link_libraries(language fmt::fmt-header-only)
