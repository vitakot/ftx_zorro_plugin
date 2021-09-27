cmake_minimum_required(VERSION 3.20)
project(ftx_zorro_plugin)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.77 REQUIRED)

link_directories (OpenSSL/lib ${Boost_LIBRARY_DIRS})
include_directories(include cpp-httplib json/include OpenSSL/include SYSTEM ${Boost_INCLUDE_DIR})

add_subdirectory(cpp-httplib json)

#add_library(ftx SHARED
#        ftx.cpp
#        src/ftx_client.cpp)

add_executable(ftx_test
        main.cpp
        src/ftx_client.cpp
        src/utils.cpp)

target_link_libraries(ftx_test libcrypto libssl)
