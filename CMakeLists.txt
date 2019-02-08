cmake_minimum_required(VERSION 3.9)
project(ANurbs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/external_libraries"
)

add_subdirectory(test)

add_subdirectory(external_libraries/pybind11)

add_definitions(
    -DEIGEN_DEFAULT_TO_ROW_MAJOR
)

pybind11_add_module(ANurbs ${SOURCES}
    "${PROJECT_SOURCE_DIR}/src/Module.cc"
    "${PROJECT_SOURCE_DIR}/external_libraries/ClipperLib/clipper.cpp"
)

install(TARGETS ANurbs DESTINATION bin)