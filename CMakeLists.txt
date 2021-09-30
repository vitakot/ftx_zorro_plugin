cmake_minimum_required(VERSION 3.20)
project(ftx_zorro_plugin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "C:/Users/vitez/Zorro/Plugin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/Users/vitez/Zorro/Plugin")

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_WIN32_WINNT=0x0A00)

#find_package(Boost 1.77 REQUIRED)
#find_package(OpenSSL REQUIRED)

include_directories(include better-enums json/include SYSTEM ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
add_subdirectory(json)

set(HEADERS
        include/i_json.h)

set(SOURCES
        src/utils.cpp
        src/ftx_models.cpp
        src/stdafx.cpp
        src/sha256.cpp)

add_library(ftx SHARED dllmain.cpp src/ftx.cpp ${SOURCES} ${HEADERS})
target_compile_definitions(ftx PUBLIC FTX_DLL_EXPORTS)

#add_executable(ftx_test main.cpp ${SOURCES} ${HEADERS})

#target_link_libraries(ftx_test)
target_link_libraries(ftx)