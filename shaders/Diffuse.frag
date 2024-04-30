#version 330

// The camera's position in world-space
uniform vec3 u_cam_pos;

// Color
uniform vec4 u_color;

// Properties of the single point light
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

// We also get the uniform texture we want to use.
uniform sampler2D u_texture_1;

// These are the inputs which are the outputs of the vertex shader.
in vec4 v_position;
in vec4 v_normal;

// This is where the final pixel color is output.
// Here, we are only interested in the first 3 dimensions (xyz).
// The 4th entry in this vector is for "alpha blending" which we
// do not require you to know about. For now, just set the alpha
// to 1.
out vec4 out_color;

void main() {
    vec3 light_direction = normalize(u_light_pos - v_position.xyz);

    float diffuse_intensity = max(dot(v_normal.xyz, light_direction), 0.0);

    vec3 color = (u_light_intensity / (distance(u_light_pos, v_position.xyz) * distance(u_light_pos, v_position.xyz))) * diffuse_intensity * u_color.rgb;

    // Set the alpha value of the output color to 1
    out_color = vec4(color, 1.0);
}
