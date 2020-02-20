
#include <numeric>
#include "opengl/OpenGLHelper.h"
#include "opengl/Types.h"

namespace cso {
	namespace Harris3D {

		struct CSO {
			GLuint id_program;

			GLint loc_sdf, loc_pdf, loc_pdf_nms, loc_mode, loc_res;
		};

		inline void init(CSO& cso, std::string shader_dir) {
			// -> compile
			GLuint cs = 0;
			ct::opengl::load_shader(cs, shader_dir + "/Harris3DCS.glsl", GL_COMPUTE_SHADER);
			GLuint shaders[1] = {cs};
			ct::opengl::create_program(cso.id_program, shaders, 1);
			glUseProgram(cso.id_program);
			// <-

			// -> locations
			cso.loc_sdf = glGetUniformLocation(cso.id_program, "sdf");
			//assert(cso.loc_sdf != -1);
			cso.loc_pdf = glGetUniformLocation(cso.id_program, "pdf");
			//assert(cso.loc_pdf != -1);
			cso.loc_pdf_nms = glGetUniformLocation(cso.id_program, "pdf_nms");
			//assert(cso.loc_pdf_nms != -1);
			cso.loc_mode = glGetUniformLocation(cso.id_program, "mode");
			//assert(cso.loc_mode != -1);
			cso.loc_res = glGetUniformLocation(cso.id_program, "res");
			//assert(cso.loc_res != -1);
			// <-
		}
		
		inline void run(CSO &cso, Eigen::Vector3i dims, float res, ct::opengl::Texture &tex_sdf, ct::opengl::Texture &tex_pdf, ct::opengl::Texture &tex_pdf_nms) {
			glUseProgram(cso.id_program);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glActiveTexture(GL_TEXTURE0 + tex_sdf.unit);
			glBindTexture(GL_TEXTURE_3D, tex_sdf.id);
			glBindImageTexture(0, tex_sdf.id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

			glActiveTexture(GL_TEXTURE0 + tex_pdf.unit);
			glBindTexture(GL_TEXTURE_3D, tex_pdf.id);
			glBindImageTexture(1, tex_pdf.id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
			
			glActiveTexture(GL_TEXTURE0 + tex_pdf_nms.unit);
			glBindTexture(GL_TEXTURE_3D, tex_pdf_nms.id);
			glBindImageTexture(2, tex_pdf_nms.id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
			
			glUniform1f(cso.loc_res, res);

			glUniform1i(cso.loc_mode, 0);
			glDispatchCompute(dims(0), dims(1), dims(2));
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			
			glUniform1i(cso.loc_mode, 1);
			glDispatchCompute(dims(0), dims(1), dims(2));
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			
			glUniform1i(cso.loc_mode, 2);
			glDispatchCompute(dims(0), dims(1), dims(2));
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			
			glUniform1i(cso.loc_mode, 3);
			glDispatchCompute(dims(0), dims(1), dims(2));
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
	}
}

