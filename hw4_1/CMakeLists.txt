cmake_minimum_required(VERSION 3.10)
project(hw4_1)

set(CMAKE_CXX_STANDARD 14)

find_package( Eigen3 REQUIRED )
find_package( OpenCV REQUIRED )

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})

add_executable(hw4_1 main.cpp)

target_link_libraries( hw4_1 ${OpenCV_LIBS}  )

