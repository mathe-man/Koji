file(GLOB_RECURSE KOJI_CORE_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(KojiCore STATIC ${KOJI_CORE_SOURCES})

target_include_directories(KojiCore PUBLIC include)