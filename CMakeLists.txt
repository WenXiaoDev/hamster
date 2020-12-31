cmake_minimum_required(VERSION 3.5)

project(hamster)

set(CMAKE_CXX_FLAGS "-g -std=c++11")

include_directories(
    ./inc
)

set(SOURCE
    ./src/DemoMain.cpp
    ./src/Thread.cpp
)

add_executable(demoMain ${SOURCE})

target_link_libraries(
    demoMain
    pthread
)