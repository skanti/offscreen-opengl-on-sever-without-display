#pragma once

#include <GL/glew.h>
#include <eigen3/Eigen/Dense>

namespace ct {
	namespace Wireframe {

		static void make_wireframe(Eigen::MatrixXf &mesh,Eigen::MatrixXf &color) {
			// -> vertices
			float vertices[] = {
				-1, -1, -1,  1, -1, -1, // x
				-1, -1,  1,  1, -1,  1, // x
				-1,  1, -1,  1,  1, -1, // x
				-1,  1,  1,  1,  1,  1, // x

				-1, -1, -1,  -1,  1, -1, // y
				 1, -1, -1,   1,  1, -1, // y
				-1, -1,  1,  -1,  1,  1, // y
				 1, -1,  1,   1,  1,  1, // y

				-1, -1, -1,  -1, -1,  1, // z
				 1, -1, -1,   1, -1,  1, // z
				-1,  1, -1,  -1,  1,  1, // z
				 1,  1, -1,   1,  1,  1, // z

			};

			float colors[] = {
				1, 0, 0, 1, 0, 0,
				1, 0, 0, 1, 0, 0,
				1, 0, 0, 1, 0, 0,
				1, 0, 0, 1, 0, 0,

				0, 1, 0, 0, 1, 0,
				0, 1, 0, 0, 1, 0,
				0, 1, 0, 0, 1, 0,
				0, 1, 0, 0, 1, 0,

				0, 0, 1, 0, 0, 1,
				0, 0, 1, 0, 0, 1,
				0, 0, 1, 0, 0, 1,
				0, 0, 1, 0, 0, 1,
			};

			int n_vertices = 24;

			mesh.resize(3, n_vertices);
			color.resize(3, n_vertices);
			std::copy(vertices, vertices + 3*n_vertices, mesh.data());
			std::copy(colors, colors + 3*n_vertices, color.data());
			mesh *= 0.5;
		}

	}
}
