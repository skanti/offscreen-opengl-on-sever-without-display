#version 450

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

uniform sampler2D image_raw;

layout(rgba8, binding = 0) writeonly restrict uniform image2D image_filtered;

#define CMP(a, b) a < b
#define SWAP(a, b) tmp = a; a = b; b = tmp;
#define CSWAP(a, b) if (CMP(a[0], b[0])) {SWAP(a[0], b[0])}; if (CMP(a[1], b[1])) {SWAP(a[1], b[1])}; if (CMP(a[2], b[2])) {SWAP(a[2], b[2])};

vec3 buff[9];
float tmp;

void main() {
    const ivec2 u = ivec2(gl_GlobalInvocationID.xy);

	buff[0] = texelFetch(image_raw, u + ivec2(0, 1), 0).rgb;
	buff[1] = texelFetch(image_raw, u + ivec2(1, 1), 0).rgb;
	buff[2] = texelFetch(image_raw, u + ivec2(1, 0), 0).rgb;
	buff[3] = texelFetch(image_raw, u + ivec2(1, -1), 0).rgb;
	buff[4] = texelFetch(image_raw, u + ivec2(0, -1), 0).rgb;
	buff[5] = texelFetch(image_raw, u + ivec2(-1, -1), 0).rgb;
	buff[6] = texelFetch(image_raw, u + ivec2(-1, 0), 0).rgb;
	buff[7] = texelFetch(image_raw, u + ivec2(-1, 1), 0).rgb;
	buff[8] = texelFetch(image_raw, u, 0).rgb;
	
	CSWAP(buff[0], buff[1]);
	CSWAP(buff[2], buff[3]);
	CSWAP(buff[0], buff[2]);
	CSWAP(buff[1], buff[3]);
	CSWAP(buff[1], buff[2]);
	CSWAP(buff[4], buff[5]);
	CSWAP(buff[7], buff[8]);
	CSWAP(buff[6], buff[8]);
	CSWAP(buff[6], buff[7]);
	CSWAP(buff[4], buff[7]);
	CSWAP(buff[4], buff[6]);
	CSWAP(buff[5], buff[8]);
	CSWAP(buff[5], buff[7]);
	CSWAP(buff[5], buff[6]);
	CSWAP(buff[0], buff[5]);
	CSWAP(buff[0], buff[4]);
	CSWAP(buff[1], buff[6]);
	CSWAP(buff[1], buff[5]);
	CSWAP(buff[1], buff[4]);
	CSWAP(buff[2], buff[7]);
	CSWAP(buff[3], buff[8]);
	CSWAP(buff[3], buff[7]);
	CSWAP(buff[2], buff[5]);
	CSWAP(buff[2], buff[4]);
	CSWAP(buff[3], buff[6]);
	CSWAP(buff[3], buff[5]);
	CSWAP(buff[3], buff[4]);

	imageStore(image_filtered, u, vec4(buff[4], 1));
}

