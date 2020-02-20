
#include <eigen3/Eigen/Dense>

namespace ct {
	namespace Icosahedron {
		void make_icosahedron(Eigen::Matrix<float, -1, -1> &V, Eigen::Matrix<uint32_t, -1, -1> &F) {

			int n_vertices = 12;
			int n_elements = 20;

			V.resize(n_vertices, 3);
		    V << 0,   -0.5257,    0.8507,
				0.8507,         0,    0.5257,
				0.8507,         0,   -0.5257,
			   -0.8507,         0,   -0.5257,
			   -0.8507,         0,    0.5257,
			   -0.5257,    0.8507,         0,
				0.5257,    0.8507,         0,
				0.5257,   -0.8507,         0,
			   -0.5257,   -0.8507,         0,
					 0,   -0.5257,   -0.8507,
					 0,    0.5257,   -0.8507,
					 0,    0.5257,    0.8507;
			V.transposeInPlace();

			F.resize(n_elements, 3);
			F << 1,     2,     6,
				 1,     7,     2,
				 3,     4,     5,
				 4,     3,     8,
				 6,     5,    11,
				 5,     6,    10,
				 9,    10,     2,
				10,     9,     3,
				 7,     8,     9,
				 8,     7,     0,
				11,     0,     1,
				 0,    11,     4,
				 6,     2,    10,
				 1,     6,    11,
				 3,     5,    10,
				 5,     4,    11,
				 2,     7,     9,
				 7,     1,     0,
				 3,     9,     8,
				 4,     8,     0;
			F.transposeInPlace();

		}
	}
}
