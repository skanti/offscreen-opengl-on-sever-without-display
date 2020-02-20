#version 450

in vec2 frag_uv;

uniform sampler2D image;
out vec4 frag_color;

void main() {
   frag_color = texture(image, frag_uv);
}
