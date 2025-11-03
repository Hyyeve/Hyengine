set(H_SOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/sources)
set(H_TARGETS Hyengine glad stblib demo)

add_subdirectory(sources/glad)
add_subdirectory(sources/Hyengine)
add_subdirectory(sources/stblib)
add_subdirectory(sources/demo)
