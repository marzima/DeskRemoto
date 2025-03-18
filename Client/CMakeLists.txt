cmake_minimum_required(VERSION 3.14)
project(DeskRemoto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Include directory
include_directories(include)

# Esegui il client
add_executable(client src/client1.cpp)
target_link_libraries(client PRIVATE ws2_32)

