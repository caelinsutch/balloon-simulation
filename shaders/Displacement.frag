#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

// Helper function to sample the height map
float h(vec2 uv) {
    return texture(u_texture_2, uv).r;
}

void main() {
    // Calculate partial derivatives of the height map to find dU and dV
    vec2 temp = vec2(v_uv.x + 1.0 / u_texture_2_size.x, v_uv.y);
    float dU = (h(temp) - h(v_uv)) * u_height_scaling * u_normal_scaling;

    temp = vec2(v_uv.x, v_uv.y + 1.0 / u_texture_2_size.y); // Corrected parentheses
    float dV = (h(temp) - h(v_uv)) * u_height_scaling * u_normal_scaling;

    // Calculate the local space normal
    vec3 local_normal = normalize(vec3(-dU, -dV, 1.0));

    // Calculate the TBN matrix
    vec3 tangent = normalize(v_tangent.xyz);
    vec3 normal = normalize(v_normal.xyz);
    vec3 bitangent = cross(normal, tangent);

    mat3 TBN = mat3(tangent, bitangent, normal);

    // Transform the local space normal to model space
    vec3 model_space_normal = TBN * local_normal;

    // Output the final world space normal
    vec3 world_space_normal = normalize(model_space_normal);

    // Calculate lighting
    float ka = 0.0;
    float Ia = 0.5;
    float kd = 1.0;
    float ks = 0.5;
    float p = 10.0;

    vec3 light_direction = normalize(u_light_pos - v_position.xyz);
    vec3 cam_direction = normalize(u_cam_pos - v_position.xyz);
    vec3 h_vec = normalize(light_direction + cam_direction);

    float diffuse_intensity = max(dot(world_space_normal, light_direction), 0.0);
    float specular_intensity = max(dot(world_space_normal, h_vec), 0.0);
    specular_intensity = pow(specular_intensity, p);

    float dist = distance(u_light_pos, v_position.xyz);
    const float epsilon = 0.00001;
    vec3 Ir2 = u_light_intensity / (dist * dist + epsilon);

    // Calculate final lighting
    vec3 lighting = ka * Ia + kd * Ir2 * diffuse_intensity + ks * Ir2 * specular_intensity;

    // Set the output color
    out_color = vec4(lighting * u_color.xyz, u_color.a);
}
