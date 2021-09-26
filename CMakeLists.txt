cmake_minimum_required(VERSION 3.20)
project(ftx_zorro_plugin)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_subdirectory(websocketpp json)

add_library(ftx SHARED
        ftx.cpp
        src/ftx_client.cpp)

