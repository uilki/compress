cmake_minimum_required(VERSION 3.16)

project(
  compress
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(compress_OUT_DIR ${${PROJECT_NAME}_BINARY_DIR}/bin/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${compress_OUT_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${compress_OUT_DIR}/bin/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${compress_OUT_DIR}/bin/lib)

file(COPY ${CMAKE_SOURCE_DIR}/images/test-image-1-gs.bmp
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${CMAKE_SOURCE_DIR}/images/test-image-2-gs.bmp
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

add_subdirectory(src)
