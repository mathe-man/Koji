add_executable(KojiTests main.cpp version_test.cpp)

target_include_directories(KojiTests PRIVATE ${PROJECT_SOURCE_DIR}/libs/doctest)

target_link_libraries(KojiTests PRIVATE KojiCore)

add_test(NAME KojiTests COMMAND KojiTests)