# Copyright (c) 2020, SenseTime.
# Author: Hailin Yu
# Email: yuhailin@sensetime.com
# Created Time: 2020/08/19

cmake_minimum_required(VERSION 3.12)
project(panorama)

add_compile_options(-std=c++11)
add_compile_options(-fPIC)
add_compile_options(-O3)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(FreeImage REQUIRED)
find_package(Glog REQUIRED)

# Eigen
find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIRS})
    message( STATUS "EIGEN3_INCLUDE_DIRS = ${EIGEN3_INCLUDE_DIRS}")
endif()

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS filesystem system regex)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
if(OpenCV_FOUND)
    message( STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
    message( STATUS "OpenCV_LIBRARIES = ${OpenCV_LIBRARIES}")
    message( STATUS "OpenCV_LIB_VERSION = ${OpenCV_LIB_VERSION}")
endif()

add_subdirectory(src)

add_subdirectory(app)