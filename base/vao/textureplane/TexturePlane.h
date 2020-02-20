#pragma once

#include <string>
#include <GL/glew.h>
#include <eigen3/Eigen/Dense>

#include <opengl/OpenGLHelper.h>

namespace ct {
	namespace vao {
		namespace TexturePlane {

			struct VAO {
				GLuint id_program;
				GLuint id_vao;

				GLint loc_model_matrix;
				GLint loc_image;
			};

			inline void init(VAO &vao,  std::string shader_dir) {
				// -> compiling and creating program
				GLuint vs = 0, fs = 0;
				ct::opengl::load_shader(vs, shader_dir + "/TexturePlaneVS.glsl", GL_VERTEX_SHADER);
				ct::opengl::load_shader(fs, shader_dir + "/TexturePlaneFS.glsl", GL_FRAGMENT_SHADER);
				GLuint shaders[2] = {vs, fs};
				ct::opengl::create_program(vao.id_program, shaders, 2);
				glUseProgram(vao.id_program);
				glGenVertexArrays(1, &vao.id_vao);
				// <-

				// -> locations
				vao.loc_model_matrix = glGetUniformLocation(vao.id_program, "model_matrix");
				assert(vao.loc_model_matrix != -1);
				vao.loc_image = glGetUniformLocation(vao.id_program, "image");
				assert(vao.loc_image != -1);
				// <-
				
			}


			inline void draw(VAO &vao, GLuint &id_vbo_vertex, GLuint &id_vbo_uv,
					GLuint &id_tex, int i_unit, Eigen::Matrix4f &model_matrix, Eigen::Matrix4f &view_matrix, Eigen::Matrix4f &projection_matrix) {
				glUseProgram(vao.id_program);
				glBindVertexArray(vao.id_vao); 

				glBindBuffer(GL_ARRAY_BUFFER, id_vbo_vertex);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, id_vbo_uv);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glUniformMatrix4fv(vao.loc_model_matrix, 1, GL_FALSE, model_matrix.data());

				glActiveTexture(GL_TEXTURE0 + i_unit);
				glBindTexture(GL_TEXTURE_2D, id_tex);
				glUniform1i(vao.loc_image, i_unit);

				glDrawArrays(GL_TRIANGLES, 0, 6);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);

			}

		}
	}
}
