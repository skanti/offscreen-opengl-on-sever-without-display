#pragma once

#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <eigen3/Eigen/Dense>

namespace ct {
	namespace opengl {

		double timing(std::function<void(void)> f) {
			GLuint64 startTime, stopTime;
			unsigned int id_query[2];
			glGenQueries(2, id_query);
			glQueryCounter(id_query[0], GL_TIMESTAMP);
			f();
			glQueryCounter(id_query[1], GL_TIMESTAMP);
			int stopTimerAvailable = 0;
			while (!stopTimerAvailable) { glGetQueryObjectiv(id_query[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable); }
			glGetQueryObjectui64v(id_query[0], GL_QUERY_RESULT, &startTime);
			glGetQueryObjectui64v(id_query[1], GL_QUERY_RESULT, &stopTime);
			double timing = (stopTime - startTime)/1000000.0;
			glDeleteQueries(2, id_query);
			return timing;
		}


		template<typename Derived>
			inline Eigen::Matrix<typename Derived::Scalar, 4, 4> make_view_matrix(Derived const & eye, Derived const & lookat, Derived const & up){
				typedef Eigen::Matrix<typename Derived::Scalar,4,4> Matrix4;
				typedef Eigen::Matrix<typename Derived::Scalar,3,1> Vector3;
				Vector3 f = (lookat- eye).normalized();
				Vector3 u = up.normalized();
				Vector3 s = f.cross(u).normalized();
				u = s.cross(f);
				Matrix4 mat = Matrix4::Zero();
				mat(0,0) = s.x();
				mat(0,1) = s.y();
				mat(0,2) = s.z();
				mat(0,3) = -s.dot(eye);
				mat(1,0) = u.x();
				mat(1,1) = u.y();
				mat(1,2) = u.z();
				mat(1,3) = -u.dot(eye);
				mat(2,0) = -f.x();
				mat(2,1) = -f.y();
				mat(2,2) = -f.z();
				mat(2,3) = f.dot(eye);
				mat.row(3) << 0,0,0,1;
				return mat;
			}

		template<typename Scalar>
			inline Eigen::Matrix<Scalar, 4, 4> make_ortho_matrix( Scalar const& left, Scalar const& right, Scalar const& bottom,Scalar const& top,
					Scalar const& zNear, Scalar const& zFar ) {
				Eigen::Matrix<Scalar,4,4> mat = Eigen::Matrix<Scalar,4,4>::Identity();
				mat(0,0) = Scalar(2) / (right - left);
				mat(1,1) = Scalar(2) / (top - bottom);
				mat(2,2) = - Scalar(2) / (zFar - zNear);
				mat(3,0) = - (right + left) / (right - left);
				mat(3,1) = - (top + bottom) / (top - bottom);
				mat(3,2) = - (zFar + zNear) / (zFar - zNear);
				return mat;
			}

		template<typename Scalar>
			inline Eigen::Matrix<Scalar, 4, 4> make_perspective_matrix(Scalar fovy, Scalar aspect, Scalar zNear, Scalar zFar){
				Eigen::Transform<Scalar, 3, Eigen::Projective> tr;
				tr.matrix().setZero();
				assert(aspect > 0);
				assert(zFar > zNear);
				assert(zNear > 0);
				Scalar radf = M_PI * fovy / 180.0;
				Scalar tan_half_fovy = std::tan(radf / 2.0);
				tr(0,0) = 1.0 / (aspect * tan_half_fovy);
				tr(1,1) = 1.0 / (tan_half_fovy);
				tr(2,2) = - (zFar + zNear) / (zFar - zNear);
				tr(3,2) = - 1.0;
				tr(2,3) = - (2.0 * zFar * zNear) / (zFar - zNear);
				return tr.matrix();
			}

		inline void init_gl(std::string title, int width, int height, GLFWwindow **window) {
			if (!glfwInit())
				throw std::runtime_error("glfwInit failed");

			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

			*window = glfwCreateWindow(width, height, title.c_str(), 0, 0);
			if (!*window)
				throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 4.5?");

			glfwMakeContextCurrent(*window);

			if (glewInit() != GLEW_OK)
				throw std::runtime_error("glewInit failed\n");

			std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
			std::cout << "WindowManager: " << glGetString(GL_RENDERER) << std::endl;
			std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
			std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		}

		inline void load_text_from_file(std::string filename, std::string &content) {
			std::ifstream in(filename);
			content = "";
			std::string line = "";
			if (in.is_open()) {
				while (std::getline(in, line)) {
					content += line + "\n";
				}
				in.close();
			}
		}

		inline void set_dummy_key_callback(GLFWwindow *window) {
			auto key_callback = [](GLFWwindow *, int, int, int, int) {
				return;
			};
			glfwSetKeyCallback(window, key_callback);
		}

		inline void clear_screen() {
			glClearColor(0.6, 0.7, 0.6f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		inline int is_window_alive(GLFWwindow *window) {
			return !glfwWindowShouldClose(window);
		}

		inline void poll_events() {
			glfwPollEvents();
		}

		inline void swap_window_buffer(GLFWwindow *window) {
			glfwSwapBuffers(window);
		}

		inline void terminate_window() {
			glfwTerminate();
		}

		inline void load_shader(GLuint &id, std::string path, GLenum type) {
			id = glCreateShader(type);

			std::string src0;
			load_text_from_file(path, src0);
			const char* src = src0.c_str();
			glShaderSource(id, 1, &src, NULL);

			glCompileShader(id);

			GLint status;
			glGetShaderiv(id, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE) {
				std::string msg("Compile failure in shader " + path + "\n");

				GLint infoLogLength;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
				char *strInfoLog = new char[infoLogLength + 1];
				glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog);
				msg += strInfoLog;
				delete[] strInfoLog;

				std::cout << msg << std::endl;
				glDeleteShader(id);

			}
		}

		inline void create_program(GLuint &program, GLuint *shaders, int n) {

			program = glCreateProgram();
			assert(program);

			for (int i = 0; i < n; i++)
				glAttachShader(program, shaders[i]);

			glLinkProgram(program);

			for (int i = 0; i < n; i++)
				glDetachShader(program, shaders[i]);


			GLint status;
			glGetProgramiv(program, GL_LINK_STATUS, &status);

			if (status == GL_FALSE) {
				std::string msg("Program linking failure: ");

				GLint infoLogLength;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
				char *strInfoLog = new char[infoLogLength + 1];
				glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
				msg += strInfoLog;
				delete[] strInfoLog;

				std::cout << msg << std::endl;
				glDeleteProgram(program);

			}
		}

		template <typename T>
			inline void create_and_upload_vbo(GLuint &id_vbo, Eigen::Matrix<T, -1, -1> &data) {
				glGenBuffers(1, &id_vbo);
				glBindBuffer(GL_ARRAY_BUFFER, id_vbo);
				glBufferData(GL_ARRAY_BUFFER, data.rows()*data.cols()*sizeof(T), data.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

		inline void fence() {
             GLsync fence;
             GLenum wait;
             fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
             while(1) {wait = glClientWaitSync(fence,0, (GLuint64)(1000)); if (wait == GL_ALREADY_SIGNALED || wait == GL_CONDITION_SATISFIED) {break;}}
		 }
	}
}
