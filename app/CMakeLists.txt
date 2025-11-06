project(KojiTestApp LANGUAGES CXX)

add_executable(KojiTestApp src/main.cpp)

target_link_libraries(KojiTestApp PRIVATE KojiEngine)
target_include_directories(KojiTestApp PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)