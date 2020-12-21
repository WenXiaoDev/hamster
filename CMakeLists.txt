# Hamster CMakeList.txt
cmake_minimum_required(VERSION 3.5)
project(hamster_demo)

set(SOURCES
    src/DemoMain.cpp
    src/Thread.cpp
    src/ThreadPool.cpp
)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(
    hamster_demo
    PRIVATE
        ${PROJECT_SOURCE_DIR}/inc
)