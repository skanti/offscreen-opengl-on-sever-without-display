#pragma once

#include <GL/glew.h>
#include <eigen3/Eigen/Dense>

namespace ct {
	namespace vao {
		namespace SimpleMeshElements {

			struct VAO {
				GLuint id_program;
				GLuint id_vao, id_vbo_vertices, id_vbo_colors, id_ebo;

				int loc_model_matrix, loc_view_matrix, loc_projection_matrix;

				uint32_t n_size_elements;
			};

			inline static void init(VAO &vao, std::string shader_dir) {

				GLuint vs = 0, fs = 0;
				ct::opengl::load_shader(vs, shader_dir + "/SimpleMeshElementsVS.glsl", GL_VERTEX_SHADER);
				ct::opengl::load_shader(fs, shader_dir + "/SimpleMeshElementsFS.glsl", GL_FRAGMENT_SHADER);
				GLuint shaders[2] = {vs, fs};
				ct::opengl::create_program(vao.id_program, shaders, 2);
				glUseProgram(vao.id_program);
				glGenVertexArrays(1, &vao.id_vao);

				vao.loc_model_matrix = glGetUniformLocation(vao.id_program, "model_matrix");
				assert(vao.loc_model_matrix != -1);
				vao.loc_view_matrix = glGetUniformLocation(vao.id_program, "view_matrix");
				assert(vao.loc_view_matrix != -1);
				vao.loc_projection_matrix = glGetUniformLocation(vao.id_program, "projection_matrix");
				assert(vao.loc_projection_matrix != -1);
			}

			inline static void upload_data(VAO &vao, Eigen::MatrixXf &vertices, Eigen::MatrixXf &colors, Eigen::Matrix<uint32_t, -1, -1> &elements) {
				// -> vertices (vbo)
				glGenBuffers(1, &vao.id_vbo_vertices);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertices);
				glBufferData(GL_ARRAY_BUFFER, 3*vertices.cols()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				// <-

				// -> color (vbo)
				glGenBuffers(1, &vao.id_vbo_colors);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_colors);
				glBufferData(GL_ARRAY_BUFFER, colors.cols()*3*sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// <-

		    	// -> elements (buffer object)
				vao.n_size_elements = elements.cols();
		        glGenBuffers(1, &vao.id_ebo);
		        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.id_ebo);
		        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.cols()*3*sizeof(GLuint), elements.data(), GL_STATIC_DRAW);
		        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		        // <-
			}

			inline static void draw(VAO& vao, Eigen::Matrix4f &model_matrix, Eigen::Matrix4f &view_matrix, Eigen::Matrix4f &projection_matrix) {
				glDisable(GL_CULL_FACE);
				glEnable(GL_DEPTH_TEST);

				glUseProgram(vao.id_program);
				glBindVertexArray(vao.id_vao);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertices);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_colors);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glUniformMatrix4fv(vao.loc_model_matrix, 1, GL_FALSE, model_matrix.data());
				glUniformMatrix4fv(vao.loc_view_matrix, 1, GL_FALSE, view_matrix.data());
				glUniformMatrix4fv(vao.loc_projection_matrix, 1, GL_FALSE, projection_matrix.data());

		        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.id_ebo);
				glDrawElements(GL_TRIANGLES, 3*vao.n_size_elements, GL_UNSIGNED_INT, 0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);

			}

		}

	}
}
