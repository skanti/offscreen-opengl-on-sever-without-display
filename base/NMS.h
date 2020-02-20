#include <eigen3/Eigen/Dense> 
#include <vector>
#include "Types.h"

#define INDEX(z, y, x) ((z)*dims(1)*dims(0) + (y)*dims(0) + (x))
#define COMPARE3(dz, dy, dx) (val < ptr[INDEX(z + dz,  y + dy, x + dx)])
#define CHECK3() \
		  (COMPARE3(-1, -1, -1) || COMPARE3(-1, -1, 0) || COMPARE3(-1, -1, 1) \
		||	COMPARE3(-1, 0, -1)  || COMPARE3(-1, 0, 0)  || COMPARE3(-1, 0, 1) \
		||	COMPARE3(-1, 1, -1)  || COMPARE3(-1, 1, 0)  || COMPARE3(-1, 1, 1) \
		||	COMPARE3(0, -1, -1)  || COMPARE3(0, -1, 0)  || COMPARE3(0, -1, 1) \
		||	COMPARE3(0, 0,  -1)   || COMPARE3(0, 0, 0)   || COMPARE3(0, 0, 1) \
		||	COMPARE3(0, 1,  -1)   || COMPARE3(0, 1, 0)   || COMPARE3(0, 1, 1) \
		||	COMPARE3(1, -1, -1) || COMPARE3(1, -1, 0) || COMPARE3(1, -1, 1) \
		||	COMPARE3(1, 0,  -1)  || COMPARE3(1, 0, 0)  || COMPARE3(1, 0, 1) \
		||	COMPARE3(1, 1,  -1)  || COMPARE3(1, 1, 0)  || COMPARE3(1, 1, 1)) 

//inline void nms3(Vox &vox, std::vector<Eigen::Vector3i> &out) {
inline void nms3(Vox &vox, std::vector<float> &out) {
	Eigen::Vector3i dims = vox.grid_dims;
	auto& ptr = vox.pdf;
	out = std::vector<float> (vox.pdf.size(), 0);

	for (int z = 1; z < dims(2) - 1; z++) {
		for (int y = 1; y <  dims(1) - 1; y++) {
			for (int x = 1; x < dims(0) - 1; x++) {
				const float val = ptr[INDEX(z, y, x)];
				out[INDEX(z, y, x)] = !CHECK3() ? val : 0;
			}
		}
	}
}

