file(GLOB_RECURSE KOJI_TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(KojiTests ${KOJI_TEST_SOURCES})

target_include_directories(KojiTests PRIVATE ${PROJECT_SOURCE_DIR}/libs/doctest)

target_link_libraries(KojiTests PRIVATE KojiCore)

add_test(NAME KojiTests COMMAND KojiTests)