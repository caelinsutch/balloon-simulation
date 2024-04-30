#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
    float ka = 0.0;
    float Ia = 1.0;

    float kd = 0.0;

    float ks = 1.0;
    float p = 10;

    vec3 light_direction = normalize(u_light_pos - v_position.xyz);
    vec3 cam_direction = normalize(u_cam_pos - v_position.xyz);
    
    // Correct calculation of the half vector
    vec3 h = normalize(light_direction + cam_direction);

    float diffuse_intensity = max(dot(v_normal.xyz, light_direction), 0.0);
    float specular_intensity = max(dot(v_normal.xyz, h), 0.0);
    specular_intensity = pow(specular_intensity, p);

    float dist = distance(u_light_pos, v_position.xyz);

    const float epsilon = 0.00001;
    vec3 Ir2 = u_light_intensity / (dist * dist + epsilon);

    vec3 lighting = ka * Ia + kd * Ir2 * diffuse_intensity + ks * Ir2 * specular_intensity;

    // Set the alpha value of the output color to 1
    out_color = vec4(lighting * u_color.xyz, 1.0);
}
