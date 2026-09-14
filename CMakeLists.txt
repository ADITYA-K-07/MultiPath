cmake_minimum_required(VERSION 3.16)
project(MultiPath)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_build src/main.cpp)
target_include_directories(test_build PRIVATE include)
