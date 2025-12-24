set(H_SOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/sources)
set(H_TARGETS hyengine glad stblib pcg demo)

add_subdirectory(sources/hyengine)
add_subdirectory(sources/glad)
add_subdirectory(sources/stblib)
add_subdirectory(sources/pcg)
add_subdirectory(sources/demo)
