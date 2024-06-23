cmake_minimum_required(VERSION 3.22)
project(gnu-lexer VERSION 1.0 LANGUAGES CXX)
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 23)

include_directories(include)
add_subdirectory(src)

option(BUILD_DEMO "Build demo code snippets." ON)
if (BUILD_DEMO)
  add_subdirectory(demo)
endif()
