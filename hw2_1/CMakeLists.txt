cmake_minimum_required(VERSION 3.10)
project(hw2_1)

set(CMAKE_CXX_STANDARD 14)

include_directories(${OpenCV_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )

add_executable(hw2_1 main.cpp)

target_link_libraries(  hw2_1 ${OpenCV_LIBS}  )
