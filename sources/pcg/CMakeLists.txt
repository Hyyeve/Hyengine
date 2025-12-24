add_library(pcg STATIC)

target_sources(pcg PUBLIC FILE_SET HEADERS BASE_DIRS ${H_SOURCES_ROOT} FILES

        include/pcg_extras.hpp
        include/pcg_random.hpp
        include/pcg_uint128.hpp

)

target_include_directories(pcg PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/pcg>
)

set_target_properties(pcg PROPERTIES LINKER_LANGUAGE CXX)
