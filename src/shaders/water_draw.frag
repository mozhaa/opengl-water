#version 330 core

layout (location = 0) out vec4 out_color;

uniform sampler2D envmap;
uniform sampler2D pool_texture;

uniform vec3 sun_direction;
uniform vec3 sun_color;
uniform float power;
uniform float specular_strength;

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
    vec3 view_dir = normalize(position - camera_position);

    float cos_theta = dot(-view_dir, normal);
    if (cos_theta < 0)
        discard;
    float theta = acos(cos_theta);
    float alpha = asin(sin(theta) * air_eta / water_eta);
    float h = sin(theta - alpha) / (sin(alpha) + 0.0000001);
    vec3 refracted_dir = normalize(view_dir - normal * h);

    float specular_factor = pow(max(0.0, dot(reflect(normalize(-sun_direction), normal), -view_dir)), power);
    vec3 specular_light = specular_strength * sun_color * specular_factor;

    vec3 reflected_color = get_envmap(reflect(view_dir, normal)) + specular_light;
    vec3 refracted_color = get_envmap(refracted_dir);

    vec2 texcoord = get_pool_texcoord(position, refracted_dir);
    if (texcoord.x > 0.0 && texcoord.x < 1.0 && texcoord.y > 0.0 && texcoord.y < 1.0)
        refracted_color = texture(pool_texture, texcoord).xyz;
    else
        refracted_color = get_envmap(refracted_dir);

    float r = reflect_ratio(theta);
    
    out_color = vec4(r * reflected_color + (1 - r) * refracted_color, 1.0);
}