#include <memory>

#include "../Hyengine/core/hyengine.hpp"
#include "../Hyengine/graphics/font/font_meta.hpp"
#include "Hyengine/common/colors.hpp"
#include "Hyengine/common/math.hpp"
#include "Hyengine/core/file_io.hpp"
#include "Hyengine/graphics/renderers/basic_renderer.hpp"
#include "Hyengine/graphics/textures/texture_buffer.hpp"
#include "Hyengine/input/input.hpp"
#include "Hyengine/threading/threading.hpp"

hyengine::native_window* window;
hyengine::basic_renderer renderer;
hyengine::camera cam;

bool update(const hyengine::frame_loop::loop_data& data)
{
    cam.start_update();

    float speed = hyengine::key_shift_pressed() ? 3 : 1;
    float delta_time = data.delta_time;
    if (hyengine::key_control_pressed()) speed /= 10;
    if (hyengine::key_alt_pressed()) speed /= 100;

    if (hyengine::key_pressed(GLFW_KEY_W)) cam.move_relative(0, 0, delta_time * speed);
    if (hyengine::key_pressed(GLFW_KEY_A)) cam.move_relative(-delta_time * speed, 0, 0);
    if (hyengine::key_pressed(GLFW_KEY_S)) cam.move_relative(0, 0, -delta_time * speed);
    if (hyengine::key_pressed(GLFW_KEY_D)) cam.move_relative(delta_time * speed, 0, 0);
    if (hyengine::key_pressed(GLFW_KEY_Q)) cam.move_relative(0, -delta_time * speed, 0);
    if (hyengine::key_pressed(GLFW_KEY_E)) cam.move_relative(0, delta_time * speed, 0);

    if (hyengine::key_pressed_this_frame(GLFW_KEY_R))
    {
        hyengine::shader::clear_all_shader_caches();
        renderer.reload_shaders();
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
    return !window->should_close();
}

void render(const hyengine::frame_loop::loop_data& data)
{
    hyengine::clear_buffers();
    cam.refresh(data.interpolation);
    renderer.update_shader_uniforms(data.interpolation, cam);
    renderer.draw();
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

    hyengine::enable_gl_debug();

    hyengine::font_meta test_font_meta("font.meta:Buycat");
    test_font_meta.load();

    hyengine::asset_image_data texture_data = hyengine::load_asset_image("font.image:Buycat");

    hyengine::texture_buffer font_texture;
    font_texture.allocate(GL_TEXTURE_2D, {texture_data.width, texture_data.height, 1}, 1, GL_RGBA8, 0);
    font_texture.upload_data_2d(0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data.data);
    font_texture.set_anisotropy_level(8.0);
    font_texture.set_texture_wrap_x(GL_CLAMP_TO_EDGE);
    font_texture.set_texture_wrap_y(GL_CLAMP_TO_EDGE);
    font_texture.set_upscale_filter(GL_LINEAR);
    font_texture.set_downscale_filter(GL_LINEAR);
    delete texture_data.data;

    renderer.allocate(1);
    renderer.texture(true, 0);
    font_texture.bind(0);
    renderer.bind();
    renderer.block_ready();
    renderer.rect({-1, -1}, {1, 1}, hyengine::WHITE);
    renderer.finish();
    renderer.update_shader_uniforms(0.0, cam);

    hyengine::set_cull_face_enabled(false);

    hyengine::frame_loop::config loop_config;
    loop_config.update = update;
    loop_config.render = render;
    loop_config.name = "Main Loop";
    loop_config.target_fps = 9999;

    hyengine::run_frame_loop(loop_config);
    hyengine::release_threadpool();

    renderer.free();
    font_texture.free();


    return 0;
}
