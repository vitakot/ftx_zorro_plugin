cmake_minimum_required(VERSION 3.20)
project(ftx_zorro_plugin)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(Boost 1.77 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(include better-enums json/include SYSTEM ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
add_subdirectory(json)

set(HEADERS
        include/i_json.h)

set(SOURCES
        src/ftx_client.cpp
        src/utils.cpp
        src/http_session.cpp
        src/ftx_models.cpp)

add_library(ftx SHARED ftx.cpp ${SOURCES} ${HEADERS})

add_executable(ftx_test main.cpp ${SOURCES} ${HEADERS})

target_link_libraries(ftx_test OpenSSL::Crypto OpenSSL::SSL)
target_link_libraries(ftx OpenSSL::Crypto OpenSSL::SSL)