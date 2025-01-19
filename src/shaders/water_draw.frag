#version 330 core

layout (location = 0) out vec4 out_color;

uniform sampler2D envmap;
uniform sampler2D pool_texture;

uniform vec3 ambient_strength;
uniform vec3 diffuse_strength;
uniform vec3 specular_strength;

uniform float power;

uniform vec3 sun_direction;
uniform vec3 sun_color;

uniform vec3 camera_position;

in vec3 position;
in vec3 normal;

const float PI = 3.141592653589793;
const float R0 = 0.020373187841971;
const float water_eta = 1.333;
const float air_eta = 1.0;


vec3 get_envmap(vec3 dir) {
    float x = atan(dir.z, dir.x) / PI * 0.5 + 0.5;
    float y = -atan(dir.y, length(dir.xz)) / PI + 0.5;
    return texture(envmap, vec2(x, y)).xyz;
}

float reflect_ratio(float theta) {
    return R0 + (1 - R0) * pow((1 - cos(theta)), 5.0);
}

vec2 get_pool_texcoord(vec3 position, vec3 dir) {
    float k = -position.y / dir.y;
    return (position.xz + k * dir.xz) * 0.5 + vec2(0.5);
}

void main() {
    vec3 albedo = vec3(0.2, 0.5, 1.0);

    float cosine = dot(normal, normalize(sun_direction));
    float light_factor = max(0.0, cosine);
    vec3 reflect_dir = 2.0 * normal * cosine - normalize(sun_direction);
    
    vec3 view_dir = normalize(position - camera_position);

    float theta = acos(dot(-view_dir, normal));
    float alpha = asin(sin(theta) * air_eta / water_eta);
    float h = sin(theta - alpha) / (sin(alpha) + 0.0000001);
    vec3 refracted_dir = normalize(view_dir - normal * h);

    vec3 ambient_light = albedo * ambient_strength;
    vec3 diffuse_light = sun_color * albedo * light_factor;
    vec3 specular_light = specular_strength * sun_color * pow(max(0.0, dot(reflect_dir, -view_dir)), power);

    vec3 water_color = ambient_light + diffuse_light + specular_light;
    vec3 reflected_color = get_envmap(reflect(view_dir, normal));
    vec3 refracted_color = get_envmap(refracted_dir);

    vec2 texcoord = get_pool_texcoord(position, refracted_dir);
    if (texcoord.x > 0.0 && texcoord.x < 1.0 && texcoord.y > 0.0 && texcoord.y < 1.0)
        refracted_color = texture(pool_texture, texcoord).xyz;
    else
        refracted_color = get_envmap(refracted_dir);

    float r = reflect_ratio(theta);
    
    out_color = vec4(r * reflected_color + (1 - r) * refracted_color, 1.0);
    // out_color = vec4(0.1 * water_color + 0.9 * (reflect_ratio * reflected_color + (1 - reflect_ratio) * refracted_color), 1.0);
}