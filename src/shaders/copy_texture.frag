#version 330

layout (location = 0) out vec4 out_color;

uniform sampler2D src;

in vec2 texcoord;

void main() {
    out_color = texture(src, texcoord);
}