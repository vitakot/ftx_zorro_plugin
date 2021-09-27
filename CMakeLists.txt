cmake_minimum_required(VERSION 3.20)
project(ftx_zorro_plugin)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.77 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(include cpp-httplib json/include SYSTEM ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

add_subdirectory(json)

#add_library(ftx SHARED
#        ftx.cpp
#        src/ftx_client.cpp)

add_executable(ftx_test
        main.cpp
        src/ftx_client.cpp
        src/utils.cpp)

target_link_libraries(ftx_test OpenSSL::Crypto OpenSSL::SSL)
