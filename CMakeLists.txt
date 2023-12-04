cmake_minimum_required(VERSION 3.5)

project (Runner LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 20)

## Network library
add_subdirectory(network)

## Game executable
add_subdirectory(game)