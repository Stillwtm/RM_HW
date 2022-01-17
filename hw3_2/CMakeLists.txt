cmake_minimum_required(VERSION 3.10)
project(hw3_2)

set(CMAKE_CXX_STANDARD 14)

find_package( Eigen3 REQUIRED )
find_package( OpenCV REQUIRED )

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})

#include_directories("usr/include/eigen3")

add_executable(hw3_2 main.cpp)

target_link_libraries( hw3_2 ${OpenCV_LIBS}  )
