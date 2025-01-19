#version 330

layout (location = 0) in vec2 in_position;

uniform sampler2D heights_texture;
// uniform sampler2DArray heights_texture;
// uniform float heights_texture_i;
uniform vec3 sun_direction;

const float PI = 3.141592653589793;
const float R0 = 0.020373187841971;
const float water_eta = 1.333;
const float air_eta = 1.0;

vec2 get_pool_texcoord(vec3 position, vec3 dir) {
    float k = -position.y / dir.y;
    return (position.xz + k * dir.xz) * 0.5 + vec2(0.5);
}

out vec3 sky_pos;
out vec3 pool_pos;

void main() {
    vec4 texvalue = texture(heights_texture, in_position);
    // vec4 texvalue = texture(heights_texture, vec3(in_position, heights_texture_i));
    vec3 position = vec3(in_position.x * 2.0 - 1.0, texvalue.w + 0.5, in_position.y * 2.0 - 1.0);
    vec3 normal = texvalue.xyz;

    vec3 view_dir = -sun_direction;

    float theta = acos(dot(-view_dir, normal));
    float alpha = asin(sin(theta) * air_eta / water_eta);
    float h = sin(theta - alpha) / (sin(alpha) + 0.0000001);
    vec3 refracted_dir = normalize(view_dir - normal * h);

    vec2 texcoord = get_pool_texcoord(position, refracted_dir);
    gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);

    sky_pos = position - dot(position, normalize(sun_direction)) * sun_direction;
    pool_pos = vec3(texcoord.x, 0.0, texcoord.y);
}