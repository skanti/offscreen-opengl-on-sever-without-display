#version 450

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

uniform sampler2D image;

layout(rgba16f, binding = 0) writeonly restrict uniform image2D gradientx;
layout(rgba16f, binding = 1) writeonly restrict uniform image2D gradienty;


void main() {
    const int u = int(gl_GlobalInvocationID.x);
	const int v = int(gl_GlobalInvocationID.y);
	
	const vec4 dcdx = 0.5*(texelFetch(image, ivec2((u + 1), v), 0) - texelFetch(image, ivec2((u - 1), v), 0));
	const vec4 dcdy = 0.5*(texelFetch(image, ivec2(u, (v + 1)), 0) - texelFetch(image, ivec2(u, (v - 1)), 0));
	
	imageStore(gradientx, ivec2(u, v), dcdx);
	imageStore(gradienty, ivec2(u, v), dcdy);
}

