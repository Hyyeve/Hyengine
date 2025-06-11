add_library(stblib STATIC)

target_sources(stblib PRIVATE
        src/stb_libs.cpp
)

target_sources(stblib PUBLIC FILE_SET HEADERS BASE_DIRS ${H_SOURCES_ROOT} FILES
        include/stb_truetype.hpp
        include/stb_image.hpp
        include/stb_image_write.hpp
)

set_target_properties(stblib PROPERTIES LINKER_LANGUAGE CXX)