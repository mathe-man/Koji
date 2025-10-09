cmake_minimum_required(VERSION 3.13)
project(Koji)
set(CMAKE_CXX_STANDARD 17)


set(KOJI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KOJI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add include folders
include_directories(
        ${KOJI_HEADERS_DIR}
        libs/glfw/include
        libs/glm/include
        libs/bgfx/include
        libs/entt/include
        libs/ReactPhysics3D/include
)


#   Add all files from the source directory
# .h and .c files
file(GLOB_RECURSE KOJI_C_HEADER_FILES "${KOJI_HEADERS_DIR}/*.h")  # .h header files
file(GLOB_RECURSE KOJI_C_SOURCE_FILES "${KOJI_SOURCE_DIR}/*.c")  # .c source files
# .hpp and .cpp files
file(GLOB_RECURSE KOJI_CPP_HEADER_FILES "${KOJI_HEADERS_DIR}/*.hpp")  # .hpp header files
file(GLOB_RECURSE KOJI_CPP_SOURCE_FILES "${KOJI_SOURCE_DIR}/*.cpp")  # .cpp source files


# Create executable using headers and source files
add_executable(Koji
        ${KOJI_C_HEADER_FILES} ${KOJI_C_SOURCE_FILES}
        ${KOJI_CPP_HEADER_FILES} ${KOJI_CPP_SOURCE_FILES}
        )



# link GLFW
target_link_directories(Koji PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/lib")
target_link_libraries(Koji glfw3)

# link BGFX
target_link_directories(Koji PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/bgfx/lib")
target_link_libraries(Koji bgfx bimg bx)

# link ReactPhysics3D
target_link_directories(Koji PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/ReactPhysics3D/lib")
target_link_libraries(Koji reactphysics3d)