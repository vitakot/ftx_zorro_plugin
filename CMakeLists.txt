cmake_minimum_required(VERSION 3.20)
project(ftx_zorro_plugin)

# TODO: Remove
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "C:/Users/vitez/Zorro/Plugin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/Users/vitez/Zorro/Plugin")

set(CMAKE_CXX_STANDARD 20)
add_definitions(-D_WIN32_WINNT=0x0A00)

include_directories(include better-enums json/include SYSTEM ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
add_subdirectory(json)
add_subdirectory(spdlog)

set(HEADERS
        include/i_json.h
        include/ftx_client.h
        include/ftx_models.h
        include/ftx.h)

set(SOURCES
        src/ftx_models.cpp
        src/ftx_client.cpp
        src/stdafx.cpp
        src/sha256.cpp)

add_library(ftx SHARED dllmain.cpp src/ftx.cpp ${SOURCES} ${HEADERS})
target_compile_definitions(ftx PUBLIC FTX_DLL_EXPORTS)

target_link_libraries(ftx spdlog)