cmake_minimum_required(VERSION 3.10)
project(hw1_2_3)

set(CMAKE_CXX_STANDARD 14)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )

add_executable(hw1_2_3 main.cpp)

target_link_libraries(  hw1_2_3 ${OpenCV_LIBS}  )
