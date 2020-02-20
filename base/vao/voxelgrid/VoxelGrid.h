#pragma once

#include <GL/glew.h>
#include <eigen3/Eigen/Dense>

namespace ct {
	namespace vao {
		namespace VoxelGrid {

			struct VAO {
				GLuint id_program;
				GLuint id_vao, id_vbo_vertices, id_vbo_normals, id_vbo_positions, id_vbo_colors;
			};

			static void init(VAO &vao, std::string shader_dir) {
				GLuint vs = 0, fs = 0;
				ct::opengl::load_shader(vs, shader_dir + "/VoxelGridVS.glsl", GL_VERTEX_SHADER);
				ct::opengl::load_shader(fs, shader_dir + "/VoxelGridFS.glsl", GL_FRAGMENT_SHADER);
				GLuint shaders[2] = {vs, fs};
				ct::opengl::create_program(vao.id_program, shaders, 2);
				glUseProgram(vao.id_program);
				glGenVertexArrays(1, &vao.id_vao);

			}

			inline static void upload_data(VAO &vao, Eigen::MatrixXf &vertices, Eigen::MatrixXf &normals, Eigen::MatrixXf &positions, Eigen::MatrixXf &colors) {
				assert(vertices.rows() == 3);
				assert(normals.rows() == 3);
				assert(positions.rows() == 3);
				assert(colors.rows() == 3);

				glUseProgram(vao.id_program);
				glBindVertexArray(vao.id_vao);
				// -> vertices (vbo)
				glGenBuffers(1, &vao.id_vbo_vertices);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertices);
				glBufferData(GL_ARRAY_BUFFER, 3*vertices.cols()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);
				// <-

				// -> normals (vbo)
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glGenBuffers(1, &vao.id_vbo_normals);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_normals);
				glBufferData(GL_ARRAY_BUFFER, 3*normals.cols()*sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// <-

				// -> position (vbo)
				glGenBuffers(1, &vao.id_vbo_positions);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_positions);
				glBufferData(GL_ARRAY_BUFFER, positions.cols()*3*sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// <-

				// -> color (vbo)
				glGenBuffers(1, &vao.id_vbo_colors);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_colors);
				glBufferData(GL_ARRAY_BUFFER, colors.cols()*3*sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(3);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// <-

			}

			static void draw(VAO& vao, int n_size, float scale, Eigen::Matrix4f &model_matrix, Eigen::Matrix4f &view_matrix, Eigen::Matrix4f &projection_matrix) {
				glUseProgram(vao.id_program);
				glBindVertexArray(vao.id_vao); 

				glVertexAttribDivisor(2, 1);
				glVertexAttribDivisor(3, 1);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertices);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_normals);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_positions);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(2);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_colors);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(3);

				glUniform1f(0, scale);
				glUniformMatrix4fv(1, 1, GL_FALSE, model_matrix.data());
				glUniformMatrix4fv(2, 1, GL_FALSE, view_matrix.data());
				glUniformMatrix4fv(3, 1, GL_FALSE, projection_matrix.data());

				glDrawArraysInstanced(GL_TRIANGLES, 0, 12*3, n_size);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);

			}

		}

	}
}
