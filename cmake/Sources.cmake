set(H_SOURCES_ROOT ${CMAKE_SOURCE_DIR}/sources)
set(H_TARGETS Hyengine glad stblib)

add_subdirectory(sources/glad)
add_subdirectory(sources/Hyengine)
add_subdirectory(sources/stblib)
