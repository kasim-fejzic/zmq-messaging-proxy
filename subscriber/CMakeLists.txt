cmake_minimum_required(VERSION 3.15)
project(subscriber CXX)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)

find_package(zmqpp REQUIRED)
find_package(asio REQUIRED)
find_package(spdlog REQUIRED)

include_directories(include)

add_executable(${PROJECT_NAME} src/subscriber.cpp src/main.cpp)
target_link_libraries(${PROJECT_NAME} zmqpp::zmqpp asio::asio spdlog::spdlog)
