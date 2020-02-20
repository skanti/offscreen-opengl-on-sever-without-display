#version 450

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

uniform sampler2D image;

layout(rgba8, binding = 0) writeonly restrict uniform image2D blurred;

uniform int mode;

void main() {
    const ivec2 u = ivec2(gl_GlobalInvocationID.xy);
	
	ivec2 du = ivec2(0);
	if (mode == 0)
		du = ivec2(1, 0);
	else if (mode == 1)
		du = ivec2(0, 1);

	const vec4 a = 1.0/5.0*(texelFetch(image, u + 2*du, 0) + texelFetch(image, u + du, 0) + texelFetch(image, u, 0) + texelFetch(image, u - du, 0) + texelFetch(image, u - 2*du, 0));
	//const vec4 a = texelFetch(image, u, 0);
	
	imageStore(blurred, u, a);
}

