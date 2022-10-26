cmake_minimum_required(VERSION 3.15)
project(subscriber CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(zmqpp)
include_directories(src)
add_executable(${PROJECT_NAME} src/subscriber.cpp src/main.cpp)
target_link_libraries(${PROJECT_NAME} zmqpp::zmqpp)
