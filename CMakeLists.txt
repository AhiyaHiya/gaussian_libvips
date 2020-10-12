cmake_minimum_required(VERSION 3.0.0)

# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake	CACHE STRING "Vcpkg toolchain file")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(gaussian_libvips VERSION 0.1.0)

include(CTest)
enable_testing()

# Boost - used `sudo apt install libboost1.71-dev libboost-program-options1.71-dev`
find_package(Boost REQUIRED COMPONENTS program_options)

# OpenCV - used `sudo apt install libopencv-dev`
find_package(PkgConfig REQUIRED)
pkg_check_modules(VIPS REQUIRED vips)

add_executable(gaussian_libvips main.cpp)

target_include_directories(gaussian_libvips PUBLIC ${VIPS_INCLUDE_DIRS})
target_link_directories(gaussian_libvips PUBLIC ${VIPS_LIBRARY_DIRS})
target_link_libraries(gaussian_libvips Boost::program_options ${VIPS_LIBRARIES})
target_compile_options(gaussian_libvips PUBLIC ${VIPS_CFLAGS_OTHER})

add_definitions(${VIPS_CFLAGS} ${VIPS_CFLAGS_OTHER})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)