#<shader_type=vert>
#version 450

in vec3 vertex_position;
in vec2 vertex_uv;
in vec4 vertex_text_col;

uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;
uniform dvec3 u_camera_pos;

out vec4 main_col;
out vec4 outline_col;
out float outline_percent;
out vec2 frag_uv;

void main() {
    gl_Position = u_projection_mat * u_view_mat * vec4(vertex_position - u_camera_pos, 1.0);
    main_col = vertex_text_col;
    outline_col = vec4(vertex_uv, 1.0, 1.0);
    outline_percent = vertex_uv.x;
    frag_uv = vertex_uv;
}

#<shader_type=frag>
#version 450

in vec4 main_col;
in vec4 outline_col;
in float outline_percent;
in vec2 frag_uv;

uniform sampler2D u_texture;
uniform vec2 u_sdf_range;

out vec4 out_color;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenspace_sdf_range(sampler2D tex) {
    vec2 screen_uv_size = vec2(1.0) / fwidth(frag_uv);
    return max(0.5 * dot(vec2(10.0 / 1024.0), screen_uv_size), 1.0);
}

void main() {
    vec4 texel = texture(u_texture, frag_uv);
    float sdf_to_pixels = screenspace_sdf_range(u_texture);

    if(texel.a < 0.001f) discard;

    float sdf = median(texel.r, texel.g, texel.b) - 0.5;
    float pixel_distance = sdf * sdf_to_pixels;
    float body_opacity = smoothstep(-0.5, 0.5, (sdf + 0.0f) * sdf_to_pixels);

    float outline_opacity = smoothstep(-0.5, 0.5, (sdf + 0.4 * outline_percent) * sdf_to_pixels);

    out_color = mix(outline_col, main_col, body_opacity) * outline_opacity;
}