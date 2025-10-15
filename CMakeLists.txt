cmake_minimum_required(VERSION 3.13)
project(Koji LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


set(KOJI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KOJI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add include folders

# glfw build settings
# === Options pour GLFW ===
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland backend")
set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 backend")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build GLFW examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build GLFW tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build GLFW docs")

add_subdirectory(libs/glfw-3.4)
add_subdirectory(libs/glm-1.0.2)
add_subdirectory(libs/entt-3.15.0)
add_subdirectory(libs/bgfx.cmake)
add_subdirectory(libs/reactphysics3d-0.10.2)



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
        libs/entt-3.15.0/src
        libs/bgfx.cmake/bgfx/include
        libs/glfw-3.4/include
        libs/glm-1.0.2
)
target_link_libraries(Koji PRIVATE glfw bgfx bx)
