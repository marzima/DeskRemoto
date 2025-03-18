cmake_minimum_required(VERSION 3.14)
project(DeskRemoto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Include directory
include_directories(include)


# Esegui il server
add_executable(server src/server1.cpp)
target_link_libraries(server PRIVATE ws2_32)