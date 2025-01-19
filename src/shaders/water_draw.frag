#version 330 core

layout (location = 0) out vec4 out_color;

uniform vec3 ambient_strength;
uniform vec3 diffuse_strength;
uniform vec3 specular_strength;

uniform float power;

uniform vec3 sun_direction;
uniform vec3 sun_color;

uniform vec3 camera_position;

in vec3 position;
in vec3 normal;

void main() {
    vec3 albedo = vec3(0.2, 0.5, 1.0);

    float cosine = dot(normal, normalize(sun_direction));
    float light_factor = max(0.0, cosine);
    vec3 reflect_dir = 2.0 * normal * cosine - normalize(sun_direction);
    
    vec3 view_dir = normalize(camera_position - position);

    vec3 ambient_light = albedo * ambient_strength;
    vec3 diffuse_light = sun_color * albedo * light_factor;
    vec3 specular_light = specular_strength * sun_color * pow(max(0.0, dot(reflect_dir, view_dir)), power);

    out_color = vec4(ambient_light + diffuse_light + specular_light, 1.0);
}