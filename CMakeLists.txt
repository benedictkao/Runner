cmake_minimum_required(VERSION 3.5)

project (Runner LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 20)

## Logging library
add_subdirectory(logging)

## Network library
add_subdirectory(network)

## Game executable
add_subdirectory(game)

## Vendor libraries
add_subdirectory(vendor)