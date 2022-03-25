cmake_minimum_required(VERSION 3.20)
project(ftx_zorro_plugin)

set(CMAKE_CXX_STANDARD 20)
add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(Boost 1.77 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(include better-enums ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})


set(HEADERS
        include/ftx_api/ftx_http_session.h
        include/ftx_api/ftx_models.h
        include/ftx_api/ftx_rest_client.h
        include/ftx_api/ftx_websocket.h
        include/ftx_api/ftx_ws_client.h
        include/ftx_api/ftx_ws_stream_manager.h
        include/ftx_api/i_json.h
        include/ftx_api/utils.h
        include/spimpl.h)

set(SOURCES
        src/ftx_api/ftx_http_session.cpp
        src/ftx_api/ftx_models.cpp
        src/ftx_api/ftx_rest_client.cpp
        src/ftx_api/ftx_websocket.cpp
        src/ftx_api/ftx_ws_client.cpp
        src/ftx_api/ftx_ws_stream_manager.cpp
        src/ftx_api/utils.cpp)

add_library(FTX SHARED dllmain.cpp src/ftx.cpp src/stdafx.cpp ${SOURCES} ${HEADERS})
target_compile_definitions(FTX PUBLIC FTX_DLL_EXPORTS)

target_link_libraries(FTX PRIVATE spdlog::spdlog_header_only OpenSSL::Crypto OpenSSL::SSL nlohmann_json::nlohmann_json)
