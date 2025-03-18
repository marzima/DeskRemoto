cmake_minimum_required(VERSION 3.14)
project(DeskRemoto)

#Set c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Config eseguibili
add_executable(server src/server.cpp)
add_executable(client src/client.cpp)

