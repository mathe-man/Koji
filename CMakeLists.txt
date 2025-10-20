cmake_minimum_required(VERSION 3.13)
project(Koji LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Set variables for this project
set(KOJI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KOJI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(KOJI_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)


# glfw build settings
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland backend")
set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 backend")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build GLFW examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build GLFW tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build GLFW docs")

add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/entt)
add_subdirectory(libs/bgfx)
add_subdirectory(libs/reactphysics3d)


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

target_include_directories(Koji PUBLIC
        include
)
target_include_directories(Koji PRIVATE
        libs/entt/src
        libs/bgfx/bgfx/include
        libs/glfw/include
        libs/glm
)
target_link_libraries(Koji PRIVATE glfw bgfx bx)
