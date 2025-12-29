add_library(miniaudio STATIC)

target_sources(miniaudio PRIVATE
        miniaudio.c
)

target_sources(miniaudio PUBLIC FILE_SET HEADERS BASE_DIRS ${H_SOURCES_ROOT} FILES
        miniaudio.h
)

target_include_directories(miniaudio PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/miniaudio>
)

set_target_properties(miniaudio PROPERTIES LINKER_LANGUAGE C)
