#pragma once

#include <GL/glew.h>
#include <eigen3/Eigen/Dense>

namespace ct {
	namespace vao {
		namespace Wireframe {

			struct VAO {
				GLuint id_program;
				GLuint id_vao, id_vbo_vertex, id_vbo_color;
			};

			inline static void init(VAO &vao, std::string shader_dir) {

				GLuint vs = 0, fs = 0;
				ct::opengl::load_shader(vs, shader_dir + "/WireframeVS.glsl", GL_VERTEX_SHADER);
				ct::opengl::load_shader(fs, shader_dir + "/WireframeFS.glsl", GL_FRAGMENT_SHADER);
				GLuint shaders[2] = {vs, fs};
				ct::opengl::create_program(vao.id_program, shaders, 2);
				glUseProgram(vao.id_program);
				glGenVertexArrays(1, &vao.id_vao);
			}

			inline static void upload_data(VAO &vao, Eigen::MatrixXf &vertices, Eigen::MatrixXf &colors) {
				glUseProgram(vao.id_program);
				glBindVertexArray(vao.id_vao);

				// -> vertices vbo
				glGenBuffers(1, &vao.id_vbo_vertex);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertex);
				glBufferData(GL_ARRAY_BUFFER, 3*vertices.cols()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// <-

				// -> colors vbo
				glGenBuffers(1, &vao.id_vbo_color);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_color);
				glBufferData(GL_ARRAY_BUFFER, 3*colors.cols()*sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// <-

			}

			inline static void draw(VAO& vao, Eigen::Matrix4f &model_matrix, Eigen::Matrix4f &view_matrix, Eigen::Matrix4f &projection_matrix) {
				glUseProgram(vao.id_program);
				glBindVertexArray(vao.id_vao);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertex);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_color);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);



				glUniformMatrix4fv(glGetUniformLocation(vao.id_program, "model_matrix"), 1, GL_FALSE, model_matrix.data());
				glUniformMatrix4fv(glGetUniformLocation(vao.id_program, "view_matrix"), 1, GL_FALSE, view_matrix.data());
				glUniformMatrix4fv(glGetUniformLocation(vao.id_program, "projection_matrix"), 1, GL_FALSE, projection_matrix.data());
				glDrawArrays(GL_LINES, 0, 24);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);

			}
		}
	}
}
