project(KojiTestApp LANGUAGES CXX)

add_executable(KojiTestApp src/main.cpp)

set_target_properties(KojiTestApp PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(KojiTestApp PRIVATE KojiEngine)
target_include_directories(KojiTestApp PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)