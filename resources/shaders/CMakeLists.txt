set(KOJI_SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(KOJI_COMPILED_SHADERS_DIR ${KOJI_SHADERS_DIR}/compiled)

message(${CMAKE_CURRENT_SOURCE_DIR}/basic_vs.sc)
message(${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Exemple minimal
bgfx_compile_shaders(
        TYPE VERTEX
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/basic_vs.sc
        OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/compiled
        PLATFORM LINUX
        PROFILE 120  # glsl 1.20 par exemple
)

bgfx_compile_shaders(
        TYPE FRAGMENT
        SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/basic_fs.sc
        OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders
        PLATFORM LINUX
        PROFILE 120
)