#version 330 core

layout (location = 0) out vec4 out_color;

in vec3 sky_pos;
in vec3 pool_pos;

uniform float factor;

void main() {
    float sky_area = length(dFdx(sky_pos)) * length(dFdy(sky_pos));
    float pool_area = length(dFdx(pool_pos)) * length(dFdy(pool_pos));
    float caustic_factor = factor * sky_area / pool_area;
    out_color = vec4(1.0, 1.0, 1.0, caustic_factor);
}