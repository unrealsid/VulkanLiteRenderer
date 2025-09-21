cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(MT_Renderer)

set(CMAKE_CXX_STANDARD 20)

include (cmake/dependencies.cmake)
include(GNUInstallDirs)

add_subdirectory(code)
