cmake_minimum_required(VERSION 3.13)
project(Koji)
set(CMAKE_CXX_STANDARD 17)


set(KOJI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KOJI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include path
include_directories(${KOJI_HEADERS_DIR})

#   Add all files from the source directory
file(GLOB_RECURSE KOJI_SOURCE_FILES "${KOJI_SOURCE_DIR}/*.cpp")  # .cpp source files
file(GLOB_RECURSE KOJI_HEADER_FILES "${KOJI_HEADERS_DIR}/*.h")  # .h header files


add_executable(Koji
        ${KOJI_SOURCE_FILES}
        ${KOJI_HEADER_FILES})