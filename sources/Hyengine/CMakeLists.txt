add_library(Hyengine STATIC)

target_sources(Hyengine PRIVATE

        common/common.cpp
        common/pool_allocation_tracker.cpp
        common/rectangle_stack.cpp

        common/data/bitvector.cpp

        common/math/line.cpp
        common/math/rectangle.cpp
        common/math/frustum.cpp
        common/math/easing.cpp

        core/file_io.cpp
        core/hyengine.cpp
        core/logger.cpp
        core/ui_layout.cpp

        graphics/graphics.cpp
        graphics/native_window.cpp
        graphics/shaders/shader.cpp
        graphics/shaders/shader_batching.cpp
        graphics/camera.cpp
        graphics/gl_enums.cpp

        graphics/font/font_meta.cpp

        graphics/textures/texture_set.cpp
        graphics/textures/texture_sampler_state.cpp
        graphics/textures/render_texture.cpp
        graphics/textures/texture_buffer.cpp

        graphics/buffers/basic_framebuffer.cpp
        graphics/buffers/frame_buffer.cpp
        graphics/buffers/pool_data_buffer.cpp
        graphics/buffers/standard_data_buffer.cpp
        graphics/buffers/vertex_format_buffer.cpp

        graphics/renderers/basic_renderer.cpp

        input/input.cpp
        $<$<PLATFORM_ID:Windows>: input/native_input_windows.cpp >
        input/virtual_keyboard.cpp
        input/virtual_mouse.cpp
        input/virtual_controller.cpp

        threading/threading.cpp
)

target_sources(Hyengine PUBLIC FILE_SET HEADERS BASE_DIRS ${H_SOURCES_ROOT} FILES
        common/common.hpp
        common/colors.hpp
        common/id_generator.hpp
        common/pool_allocation_tracker.hpp
        common/rectangle_stack.hpp
        common/sized_numerics.hpp

        common/math/math.hpp
        common/math/line.hpp
        common/math/rectangle.hpp
        common/math/frustum.hpp
        common/math/easing.hpp
        common/math/interpolatable.hpp

        common/data/array3d.hpp
        common/data/nodes3d.hpp
        common/data/palettized_vector.hpp
        common/data/bitvector.hpp
        common/data/ring_buffer.hpp

        core/hyengine.hpp
        core/logger.hpp
        core/file_io.hpp
        core/ui_layout.hpp

        graphics/graphics.hpp
        graphics/native_window.hpp
        graphics/shaders/shader.hpp
        graphics/shaders/shader_batching.hpp
        graphics/camera.hpp
        graphics/vertex_formats.hpp
        graphics/gl_enums.hpp

        graphics/font/font_meta.hpp

        graphics/textures/texture_set.hpp
        graphics/textures/texture_sampler_state.hpp
        graphics/textures/render_texture.hpp
        graphics/textures/texture_buffer.hpp

        graphics/buffers/basic_framebuffer.hpp
        graphics/buffers/frame_buffer.hpp
        graphics/buffers/pool_data_buffer.hpp
        graphics/buffers/standard_data_buffer.hpp
        graphics/buffers/vertex_format_buffer.hpp
        graphics/buffers/typed_data_buffer.hpp
        graphics/buffers/typed_pool_buffer.hpp

        graphics/renderers/basic_renderer.hpp

        input/input.hpp
        input/native_input.hpp
        input/virtual_keyboard.hpp
        input/virtual_mouse.hpp
        input/virtual_controller.hpp
        library/gl.hpp
        library/glm.hpp
        threading/threading.hpp
)

# target link options ..


target_include_directories(Hyengine PUBLIC ${GLFW3_INCLUDE_DIRS})
target_include_directories(Hyengine PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(Hyengine PUBLIC ${GLM_INCLUDE_DIRS})

target_link_libraries(Hyengine PUBLIC glad)
target_link_libraries(Hyengine PUBLIC glfw)
target_link_libraries(Hyengine PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(Hyengine PUBLIC TracyClient)
target_link_libraries(Hyengine PUBLIC stblib)