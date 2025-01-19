#version 330

layout (location = 0) in vec2 in_position;

uniform sampler2D heights_texture;
// uniform sampler2DArray heights_texture;
// uniform float heights_texture_i;

uniform mat4 view;
uniform mat4 projection;

out vec3 position;
out vec3 normal;

void main() {
    vec4 texvalue = texture(heights_texture, in_position);
    normal = texvalue.xyz;
    position = vec3(in_position.x * 2.0 - 1.0, texvalue.w + 0.5, in_position.y * 2.0 - 1.0);
    gl_Position = projection * view * vec4(position, 1.0);
}