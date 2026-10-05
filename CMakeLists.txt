cmake_minimum_required(VERSION 3.16)
project(MultiPath)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_build src/main.cpp)
target_include_directories(test_build PRIVATE include)

find_package(ZLIB REQUIRED)       # finds the zlib library on your system
find_package(EXPAT REQUIRED)      # finds the expat (XML) library
find_package(BZip2 REQUIRED)      # finds the bzip2 library
find_package(Threads REQUIRED)    # finds thread support

target_link_libraries(test_build   # links all of them to your program
    ZLIB::ZLIB
    EXPAT::EXPAT
    BZip2::BZip2
    Threads::Threads
)
