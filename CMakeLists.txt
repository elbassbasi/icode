cmake_minimum_required(VERSION 3.0)

# set the project name
project(ICode VERSION 1.0)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(BUILD_RPATH_USE_ORIGIN TRUE)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#file(COPY DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_subdirectory(swtc)
add_subdirectory(icode)
add_subdirectory(launcher)


