#version 330 core

uniform sampler2D envmap;
uniform vec3 camera_position;

in vec3 position;

layout (location = 0) out vec4 out_color;

const float PI = 3.141592653589793;

void main() {
    vec3 dir = position - camera_position;
    float x = atan(dir.z, dir.x) / PI * 0.5 + 0.5;
    float y = -atan(dir.y, length(dir.xz)) / PI + 0.5;
    out_color = texture(envmap, vec2(x, y));
}