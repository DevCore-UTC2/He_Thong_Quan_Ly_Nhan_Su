cmake_minimum_required(VERSION 3.15)
project(QuanLyNhanSuWebServer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/server/third_party
)

file(GLOB_RECURSE SRC_CORE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp")

set(SERVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/server/main_server.cpp
    ${SRC_CORE_FILES}
)

add_executable(server_app ${SERVER_SOURCES})

if (WIN32)
    target_compile_definitions(server_app PRIVATE _WIN32_WINNT=0x0A00 WIN32_LEAN_AND_MEAN)
    target_link_libraries(server_app PRIVATE ws2_32 wsock32 crypt32)
else()
    target_link_libraries(server_app PRIVATE pthread)
endif()
