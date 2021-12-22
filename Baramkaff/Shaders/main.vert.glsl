#version 450 core

const vec2 vertices[] = {
    vec2(-1.0, -1.0),
    vec2(-1.0,  2.0),
    vec2( 2.0, -1.0)
};

out vec3 color;

void main() {
    color = vec3(1, 0, 0);
    gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
}