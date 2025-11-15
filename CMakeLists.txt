cmake_minimum_required(VERSION 3.13)
project(KojiEngine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

# Set variables for this project
set(KOJI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KOJI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(KOJI_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Generally required to link our external static lib into a other one

add_subdirectory(libs/reactphysics3d)

set(GLFW_BUILD_DOCS OFF)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/glad)
add_subdirectory(libs/entt)

# Set assimp parameters
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT ON)

add_subdirectory(libs/assimp)




#   Add all files from the source directory
# .h and .c files
file(GLOB_RECURSE KOJI_C_HEADER_FILES "${KOJI_HEADERS_DIR}/*.h")  # .h header files
file(GLOB_RECURSE KOJI_C_SOURCE_FILES "${KOJI_SOURCE_DIR}/*.c")  # .c source files
# .hpp and .cpp files
file(GLOB_RECURSE KOJI_CPP_HEADER_FILES "${KOJI_HEADERS_DIR}/*.hpp")  # .hpp header files
file(GLOB_RECURSE KOJI_CPP_SOURCE_FILES "${KOJI_SOURCE_DIR}/*.cpp")  # .cpp source files



# Create executable using headers and source files
add_library(KojiEngine SHARED
        ${KOJI_C_HEADER_FILES} ${KOJI_C_SOURCE_FILES}
        ${KOJI_CPP_HEADER_FILES} ${KOJI_CPP_SOURCE_FILES}
        )

target_include_directories(KojiEngine PUBLIC
        include
        libs/glfw/include
        libs/glm
        libs/glad/include
        libs/entt/src
        libs/reactphysics3d/include
        libs/assimp/include
)
target_link_libraries(KojiEngine PRIVATE 
        reactphysics3d 
        EnTT
        assimp
        glfw glm glad
)


# Generate apps in the app folder
add_subdirectory(app)