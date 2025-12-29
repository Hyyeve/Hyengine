#include <filesystem>

#include "hyengine/core/hyengine.hpp"
#include "hyengine/graphics/font/font_meta.hpp"
#include "hyengine/common/colors.hpp"
#include "hyengine/common/math/math.hpp"
#include "hyengine/core/file_io.hpp"
#include "hyengine/graphics/gl_enums.hpp"
#include "hyengine/graphics/renderers/simple_vertex_accumulator.hpp"
#include "hyengine/graphics/textures/texture_buffer.hpp"
#include "hyengine/input/input.hpp"
#include "hyengine/threading/threading.hpp"
#include "pcg/pcg_random.hpp"

hyengine::native_window* window;
hyengine::simple_vertex_accumulator vertex_accumulator;
hyengine::shader shader = hyengine::shader("hyengine.shader.basic_pos_col_tex");
hyengine::camera cam;

namespace keys = hyengine::keys;
namespace draw_modes = hyengine::draw_modes;

void update(const hyengine::frame_loop::loop_data& loop)
{
    cam.start_update();

    float speed = hyengine::key_shift_pressed() ? 3 : 1;
    float delta_time = loop.delta_time;
    if (hyengine::key_control_pressed()) speed /= 10;
    if (hyengine::key_alt_pressed()) speed /= 100;

    if (hyengine::key_pressed(keys::W)) cam.move_relative(0, 0, delta_time * speed);
    if (hyengine::key_pressed(keys::A)) cam.move_relative(-delta_time * speed, 0, 0);
    if (hyengine::key_pressed(keys::S)) cam.move_relative(0, 0, -delta_time * speed);
    if (hyengine::key_pressed(keys::D)) cam.move_relative(delta_time * speed, 0, 0);
    if (hyengine::key_pressed(keys::Q)) cam.move_relative(0, -delta_time * speed, 0);
    if (hyengine::key_pressed(keys::E)) cam.move_relative(0, delta_time * speed, 0);

    if (hyengine::key_pressed_this_frame(keys::R))
    {
        hyengine::shader::clear_all_binary_caches();
        shader.reload();
    }

    if (hyengine::mouse_clicked(1))
    {
        static glm::vec3 cam_euler = {};
        const glm::vec2 mouse_delta = hyengine::mouse_position_delta();
        cam_euler.x += glm::radians(mouse_delta.y) * 0.5;
        cam_euler.y -= glm::radians(mouse_delta.x) * 0.5;
        cam_euler.z = glm::radians(0.0);
        const glm::quat rotation = hyengine::make_euler_rotation_zxy(cam_euler);
        cam.rotate_to(rotation);
    }

    hyengine::process_inputs();
    hyengine::flush_logs();
    loop.config->should_exit = window->should_close();
}

void render(const hyengine::frame_loop::loop_data& data)
{
    hyengine::clear_buffers();
    cam.refresh(data.interpolation);
    shader.set_uniform("u_projection_mat", cam.get_projection());
    shader.set_uniform("u_view_mat", cam.get_view());
    shader.set_uniform("u_camera_pos", cam.get_position(data.interpolation));
    glDrawArrays(draw_modes::TRIANGLES, vertex_accumulator.get_vertex_start(), vertex_accumulator.get_vertex_count());
    window->swap_buffers();
}

int main()
{
    hyengine::window_config config;
    config.gl_profile_compatibility = true;
    config.vsync = false;
    config.gl_profile_debug = true;

    window = hyengine::initialize_graphics(config);
    hyengine::create_threadpool();

    cam.set_perspective(90.0f, 16.0 / 9.0f, 0.001f, 100.0f);
    cam.move_to(glm::vec3(0, 0, -1));

    hyengine::set_override_asset_directory("assets-demo");

    hyengine::font_meta test_font_meta("hyengine.font.meta.Buycat");
    test_font_meta.load();

    hyengine::asset_image_data texture_data = hyengine::load_asset_image("hyengine.font.image.Buycat");

    hyengine::texture_buffer font_texture;
    const bool did_allocate = font_texture.allocate(GL_TEXTURE_2D, {texture_data.width, texture_data.height, 1}, 1, GL_RGBA8, 0);
    if (!did_allocate)
    {
        return -1;
    }

    font_texture.upload_data_2d(0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data.data);
    font_texture.set_anisotropy_level(8.0);
    font_texture.set_texture_wrap_x(GL_CLAMP_TO_EDGE);
    font_texture.set_texture_wrap_y(GL_CLAMP_TO_EDGE);
    font_texture.set_upscale_filter(GL_LINEAR);
    font_texture.set_downscale_filter(GL_LINEAR);
    delete texture_data.data;
    font_texture.bind(0);

    const bool did_allocate_renderer = vertex_accumulator.allocate(1);
    if (!did_allocate_renderer)
    {
        return -1;
    }

    vertex_accumulator.bind_state();
    vertex_accumulator.rect({-1, -1}, {1, 1}, hyengine::colors::WHITE);
    vertex_accumulator.finish();

    const bool did_allocate_shader = shader.allocate();
    if (!did_allocate_shader)
    {
        return -1;
    }

    shader.set_sampler_slot("u_texture", 0);
    shader.use();


    hyengine::set_cull_face_enabled(false);

    hyengine::frame_loop::config loop_config;
    loop_config.update = update;
    loop_config.render = render;
    loop_config.name = "Main Loop";
    loop_config.target_fps = 9999;

    hyengine::run_frame_loop(loop_config);
    hyengine::release_threadpool();

    vertex_accumulator.free();
    font_texture.free();
    shader.free();

    return 0;
}
