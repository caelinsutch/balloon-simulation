#version 330

uniform vec3 u_cam_pos;
uniform samplerCube u_texture_cubemap;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;

out vec4 out_color;

void main() {
    // Calculate outgoing eye-ray direction
    vec3 wo = normalize(v_position.xyz - u_cam_pos);

    // Reflect the outgoing direction across the surface normal
    vec3 wi = reflect(wo, normalize(v_normal.xyz));

    // Sample the environment map for incoming direction wi
    vec4 environment_color = texture(u_texture_cubemap, wi);

    // Output the color
    out_color = environment_color;
}
