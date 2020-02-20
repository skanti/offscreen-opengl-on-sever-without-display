#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 color_vs;

void main() {
    gl_Position = projection_matrix*view_matrix*model_matrix*vec4(vertex, 1.0);
	color_vs = color;
}


