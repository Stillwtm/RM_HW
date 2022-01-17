cmake_minimum_required(VERSION 3.10)
project(hw1_1)

set(CMAKE_CXX_STANDARD 11)

include_directories(${OpenCV_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )

add_executable(hw1_1 catch_apple.cpp)

target_link_libraries(  hw1_1 ${OpenCV_LIBS}  )
