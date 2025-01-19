#version 330

out vec2 texcoord;

const vec2 VERTICES[3] = vec2[3](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

void main() {
    texcoord = VERTICES[gl_VertexID] * 0.5 + vec2(0.5);
    gl_Position = vec4(VERTICES[gl_VertexID], 0.0, 1.0);
}