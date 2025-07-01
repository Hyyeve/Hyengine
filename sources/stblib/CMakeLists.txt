add_library(stblib STATIC)

target_sources(stblib PRIVATE
        src/stb_libs.cpp
)

target_sources(stblib PUBLIC FILE_SET HEADERS BASE_DIRS ${H_SOURCES_ROOT} FILES
        include/stb_truetype.hpp
        include/stb_image.hpp
        include/stb_image_write.hpp
)


target_include_directories(stblib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/stblib>
)

set_target_properties(stblib PROPERTIES LINKER_LANGUAGE CXX)