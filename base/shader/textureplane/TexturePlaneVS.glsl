#version 450

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 uv;

uniform mat4 model_matrix;

out vec2 frag_uv;

void main() {
    gl_Position = model_matrix*vec4(vertex, 0.0, 1.0);
    frag_uv = uv;
}
