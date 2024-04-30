#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 in_position;
in vec4 in_normal;
in vec4 in_tangent;
in vec2 in_uv;

out vec4 v_position;
out vec4 v_normal;
out vec2 v_uv;
out vec4 v_tangent;

float h(vec2 uv) {
    return texture(u_texture_2, uv).r;
}

void main() {
    float height = h(in_uv) * u_height_scaling;

    vec3 displaced_position = in_position.xyz + in_normal.xyz * height;

    vec4 world_displaced_position = u_model * vec4(displaced_position, 1.0);

    vec3 world_normal = normalize(mat3(u_model) * in_normal.xyz);
    vec3 world_tangent = normalize(mat3(u_model) * in_tangent.xyz);

    v_position = world_displaced_position;
    v_normal = vec4(world_normal, 0.0);
    v_uv = in_uv;
    v_tangent = vec4(world_tangent, 0.0);

    gl_Position = u_view_projection * world_displaced_position;
}
