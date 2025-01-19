#version 330

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texcoord;

uniform mat4 view;
uniform mat4 projection;

out vec2 texcoord;
out vec3 position;
out vec3 normal;

void main() {
    gl_Position = projection * view * vec4(in_position, 1.0);
    normal = vec3(0.0, 1.0, 0.0);
    position = in_position;
    texcoord = in_texcoord;
}