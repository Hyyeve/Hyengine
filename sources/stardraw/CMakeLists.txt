add_executable(stardraw)

target_sources(stardraw PRIVATE
        demo.cpp

        api/api_backend.cpp
        api/pipeline.cpp

        gl/commands_impl_gl.cpp
        gl/backend_impl_gl.hpp gl/backend_impl_gl.cpp
        gl/types_gl.hpp
)

target_sources(stardraw PUBLIC FILE_SET HEADERS BASE_DIRS ${H_SOURCES_ROOT} FILES
        api/pipeline.hpp
        api/types.hpp
        api/api_backend.hpp
        api/polymorphic_list.hpp

        api/descriptors.hpp
        api/commands.hpp

)

target_include_directories(stardraw PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/stardraw>
)

set_target_properties(stardraw PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(stardraw PUBLIC ${GLFW3_INCLUDE_DIRS})
target_include_directories(stardraw PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(stardraw PUBLIC ${GLM_INCLUDE_DIRS})

target_link_libraries(stardraw PUBLIC glfw)
target_link_libraries(stardraw PUBLIC glad)
target_link_libraries(stardraw PUBLIC glm::glm)
target_link_libraries(stardraw PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(stardraw PUBLIC TracyClient)
