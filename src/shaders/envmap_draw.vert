#version 330 core

uniform mat4 view_projection_inverse;

vec2 VERTICES[3] = vec2[3](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

out vec3 position;

void main() {
    gl_Position = vec4(VERTICES[gl_VertexID], 0.0, 1.0);
    vec4 ndc = vec4(VERTICES[gl_VertexID], 0.0, 1.0);
    vec4 clip_space = view_projection_inverse * ndc;
    position = clip_space.xyz / clip_space.w;
}