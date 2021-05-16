cmake_minimum_required(VERSION 3.16)
project(net)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/client)

add_subdirectory(src/server)
