#<shader_type=vert>
#version 450

in vec3 vertex_position;
in vec2 vertex_uv;
in vec4 vertex_col;

uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;
uniform dvec3 u_camera_pos;

out vec4 frag_col;
out vec2 frag_uv;

void main() {
    gl_Position = u_projection_mat * u_view_mat * vec4(vertex_position - u_camera_pos, 1.0);
    frag_col = vertex_col;
    frag_uv = vertex_uv;
}

#<shader_type=frag>
#version 450

in vec4 frag_col;
in vec2 frag_uv;

uniform sampler2D u_texture;
out vec4 out_color;

void main() {
    out_color = texture(u_texture, frag_uv) * frag_col;
}