#pragma once

#include <eigen3/Eigen/Dense> 
#include <vector>
#include "Types.h"


inline static void kp_world2grid(std::vector<Eigen::MatrixXf> &keypoints0, Eigen::Matrix4f world2grid, std::vector<Eigen::MatrixXi> &keypoints) {
	keypoints.resize(keypoints0.size());
	for (int i = 0; i < (int)keypoints0.size(); i++) {
		keypoints[i].resize(keypoints0[i].rows(), keypoints0[i].cols());
		for (int j = 0; j < keypoints0[i].cols(); j++) {
			Eigen::Vector4f p;
			p << keypoints0[i].col(j), 1;
			keypoints[i].col(j) = (world2grid*p).array().round().cast<int>().head<3>();
			//int x = std::round(p(0));
			//int y = std::round(p(1));
			//int z = std::round(p(2));
		}
	}
}

inline static void crop(int x, int y, int z, Vox &vox, Eigen::Vector3i grid_dims, Vox &out) {
	//Eigen::Vector3i grid_dims(63, 63, 63);
	out.grid_dims = grid_dims;
	out.res = vox.res;
	out.grid2world = vox.grid2world;

	int n_elems = grid_dims(0)*grid_dims(1)*grid_dims(2);
	out.sdf = std::vector<float>(n_elems, -5*vox.res);
	out.pdf = std::vector<float>(n_elems, 0);

	int dimx = vox.grid_dims(0);
	int dimy = vox.grid_dims(1);
	int dimz = vox.grid_dims(2);

	for (int k = 0 ; k < grid_dims(2); k++){
		for (int j = 0; j <  grid_dims(1); j++){
			for (int i = 0; i < grid_dims(0); i++){
				int k0 = k + z - 0;
				int j0 = j + y - std::floor(grid_dims(1)/2);
				int i0 = i + x - std::floor(grid_dims(0)/2);

				if (k0 >= 0 && k0 < dimz && j0 >= 0 && j0 < dimy && i0 >= 0 && i0 < dimx) {
					int index0  = k0*dimy*dimx + j0*dimx + i0;
					int index1  = k*grid_dims(1)*grid_dims(0) + j*grid_dims(0) + i;
					out.sdf[index1] = vox.sdf[index0];
					out.pdf[index1] = vox.pdf[index0];
				}
			}
		}
	}
}

inline static void crop_center(int x, int y, int z, Vox &vox, Eigen::Vector3i grid_dims, Vox &out) {
	//Eigen::Vector3i grid_dims(63, 63, 63);
	out.grid_dims = grid_dims;
	out.res = vox.res;
	out.grid2world = vox.grid2world;
	out.grid2world.col(3) += vox.grid2world*Eigen::Vector4f(x - grid_dims(0)/2, y - grid_dims(1)/2, z - grid_dims(2)/2, 0);

	int n_elems = grid_dims(0)*grid_dims(1)*grid_dims(2);
	out.sdf = std::vector<float>(n_elems, -5*vox.res);

	int dimx = vox.grid_dims(0);
	int dimy = vox.grid_dims(1);
	int dimz = vox.grid_dims(2);

	for (int k = 0 ; k < grid_dims(2); k++){
		for (int j = 0; j <  grid_dims(1); j++){
			for (int i = 0; i < grid_dims(0); i++){
				int k0 = k + z - std::floor(grid_dims(2)/2);
				int j0 = j + y - std::floor(grid_dims(1)/2);
				int i0 = i + x - std::floor(grid_dims(0)/2);

				if (k0 >= 0 && k0 < dimz && j0 >= 0 && j0 < dimy && i0 >= 0 && i0 < dimx) {
					int index0  = k0*dimy*dimx + j0*dimx + i0;
					int index1  = k*grid_dims(1)*grid_dims(0) + j*grid_dims(0) + i;
					out.sdf[index1] = vox.sdf[index0];
				}
			}
		}
	}
}
