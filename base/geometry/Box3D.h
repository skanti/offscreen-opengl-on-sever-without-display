#pragma once

#include <eigen3/Eigen/Dense>

namespace ct {
	namespace geometry {
		namespace Box3D {

			void create(Eigen::MatrixXf &vertices0, Eigen::MatrixXf &normals0) {
				// -> vertices
				const float vertices[] = {
					-1,-1,-1, -1,-1, 1, -1, 1, 1,  // Left Side
					-1,-1,-1, -1, 1, 1, -1, 1,-1,  // Left Side
					1, 1,-1, -1,-1,-1, -1, 1,-1,  // Back Side
					1, 1,-1,  1,-1,-1, -1,-1,-1,  // Back Side
					1,-1, 1, -1,-1,-1,  1,-1,-1,  // Bottom Side
					1,-1, 1, -1,-1, 1, -1,-1,-1,  // Bottom Side
					-1, 1, 1, -1,-1, 1,  1,-1, 1,  // Front Side
					1, 1, 1, -1, 1, 1,  1,-1, 1,  // Front Side
					1, 1, 1,  1,-1,-1,  1, 1,-1,  // Right Side
					1,-1,-1,  1, 1, 1,  1,-1, 1,  // Right Side
					1, 1, 1,  1, 1,-1, -1, 1,-1,  // Top Side
					1, 1, 1, -1, 1,-1, -1, 1, 1,  // Top Side
				};
				// <-

				// -> normals
				const float normals[] = {
					-1,  0,  0, -1,  0,  0, -1,  0,  0, // Left Side
					-1,  0,  0, -1,  0,  0, -1,  0,  0, // Left Side
					0,  0, -1,  0,  0, -1,  0,  0, -1, // Back Side
					0,  0, -1,  0,  0, -1,  0,  0, -1, // Back Side
					0, -1,  0,  0, -1,  0,  0, -1,  0, // Bottom Side
					0, -1,  0,  0, -1,  0,  0, -1,  0, // Bottom Side
					0,  0,  1,  0,  0,  1,  0,  0,  1, // front Side
					0,  0,  1,  0,  0,  1,  0,  0,  1, // front Side
					1,  0,  0,  1,  0,  0,  1,  0,  0, // right Side
					1,  0,  0,  1,  0,  0,  1,  0,  0, // right Side
					0,  1,  0,  0,  1,  0,  0,  1,  0, // top Side
					0,  1,  0,  0,  1,  0,  0,  1,  0, // top Side
				};
				// <-

				int dim = 3;
				int n_vertices = 36;

				vertices0.resize(dim, n_vertices);
				normals0.resize(dim, n_vertices);
				std::copy(vertices, vertices + dim*n_vertices, vertices0.data());
				std::copy(normals, normals + dim*n_vertices, normals0.data());

			}
		}
	}
}
