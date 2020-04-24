cmake_minimum_required(VERSION 3.9)
project(anurbs)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Filesystem REQUIRED)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/external_libraries"
    "${PROJECT_SOURCE_DIR}/external_libraries/pybind11/include"
)

add_subdirectory(external_libraries/pybind11)

add_definitions(
    -DEIGEN_DEFAULT_TO_ROW_MAJOR
    -DANURBS_VERSION="${ANURBS_VERSION}"
)

pybind11_add_module(anurbs ${SOURCES}
    "${PROJECT_SOURCE_DIR}/src/Module.cc"
    "${PROJECT_SOURCE_DIR}/external_libraries/ClipperLib/clipper.cpp"
)

target_link_libraries(anurbs PRIVATE std::filesystem)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(anurbs PUBLIC -fsized-deallocation)
endif()

install(TARGETS anurbs DESTINATION bin)