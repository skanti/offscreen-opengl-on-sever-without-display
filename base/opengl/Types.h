#pragma once

#include <GL/glew.h>

namespace ct {
	namespace opengl {

		struct Texture {
			GLuint id;
			GLuint unit;
		};
		
		struct FBOTex {
			GLuint id_fbo;
			Texture tex;
			GLuint id_rbo_depth;
		};
		
		struct FBORBO {
			GLuint id_fbo;
			GLuint id_rbo_target;
			GLuint id_rbo_depth;
		};
		
		struct SSBO {
			GLuint id;
			GLvoid *ptr;
			uint64_t n_bytes;
		};
	}
}
