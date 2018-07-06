cmake_minimum_required(VERSION 3.9)
project(ANurbs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(test)
