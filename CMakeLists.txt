cmake_minimum_required(VERSION 3.13)
project(Koji LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Set variables for this project
set(KOJI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KOJI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(KOJI_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)


add_subdirectory(libs/raylib)
add_subdirectory(libs/entt)
add_subdirectory(libs/reactphysics3d)
add_subdirectory(libs/rlImGui)


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
        libs/raylib/src
        libs/reactphysics3d/include
        libs/ImGui
        libs/rlImGui
)
target_link_libraries(Koji PRIVATE raylib reactphysics3d rlImGui)
