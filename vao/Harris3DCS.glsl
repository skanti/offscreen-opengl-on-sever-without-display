#version 450

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(r32f, binding = 0) restrict uniform image3D sdf;
layout(r32f, binding = 1) restrict uniform image3D pdf;
layout(r32f, binding = 2) restrict uniform image3D pdf_nms;

uniform int mode;
uniform float res;


void clean(in ivec3 p0, in int kernel_size) {
	float s = imageLoad(sdf, p0).r;
	if (s < 0)
		imageStore(pdf, p0, vec4(-1));
}

void blur(in ivec3 p0, in int kernel_size) {
	float val = 0;
	const int k = kernel_size/2;
	for (int z = -k; z < k + 1 ; z++)
		for (int y = -k; y < k + 1; y++)
			for (int x = -k; x < k + 1; x++) {
				ivec3 p = p0 + ivec3(x, y, z); 
				val += imageLoad(sdf, p).r;
			}
	val /= kernel_size*kernel_size*kernel_size;
	imageStore(sdf, p0, vec4(val));
}

void abs_and_truncate(in ivec3 p0) {
	float s = imageLoad(sdf, p0).r;

	float val = clamp(s, 0.0, 0.15);
	imageStore(sdf, p0, vec4(val));
	
}

void harris3(in ivec3 p0) {
	const ivec3 dx = ivec3(1, 0, 0);
	const ivec3 dy = ivec3(0, 1, 0);
	const ivec3 dz = ivec3(0, 0, 1);

	mat3 M = mat3(0);

	for (int z = -1; z < 2; z++) {
		for (int y = -1; y < 2; y++) {
			for (int x = -1; x < 2; x++) {
				ivec3 p = p0 + ivec3(x, y, z); 

				vec3 dc = vec3(0);
				dc.x = (imageLoad(sdf, p + dx).r - imageLoad(sdf, p - dx).r);
				dc.y = (imageLoad(sdf, p + dy).r - imageLoad(sdf, p - dy).r);
				dc.z = (imageLoad(sdf, p + dz).r - imageLoad(sdf, p - dz).r);

				dc = normalize(dc);

				M += outerProduct(dc, dc);
			}
		}
	}

	const float det = determinant(M);
	const float trace = M[0][0] + M[1][1] + M[2][2];
	const float response = det/trace;


	if (abs(response) > 2.0)
		imageStore(pdf, p0, vec4(abs(response)));

}

void nms(in ivec3 p0, in int kernel_size) {
	float s = imageLoad(pdf, p0).r;
	if (s == 0)
		return;

	const int k = kernel_size/2;
	for (int z = -k; z < k + 1 ; z++) {
		for (int y = -k; y < k + 1; y++) {
			for (int x = -k; x < k + 1; x++) {
				ivec3 p = p0 + ivec3(x, y, z); 
				float s1 = imageLoad(pdf, p).r;
				if (s < s1)
					return;
					
			}
		}
	}

	imageStore(pdf_nms, p0, vec4(1));
}

void main() {
	const ivec3 p0 = ivec3(gl_GlobalInvocationID.xyz);
	float s = imageLoad(sdf, p0).r;
	float p = imageLoad(pdf, p0).r;
	ivec3 pad = ivec3(5, 5, 3);

	if (any(lessThan(p0, pad)) || any(greaterThan(p0, gl_NumWorkGroups - pad))) {
		imageStore(pdf_nms, p0, vec4(0));
		return;
	}


	if (mode == 0) 
		clean(p0, 5);
	else if (mode == 1)
		abs_and_truncate(p0);
	else if ((mode == 2) && abs(s) <= res && p != -1)
		harris3(p0);
	else if ((mode == 3) && abs(s) <= res && p != -1)
		nms(p0, 5);
}
