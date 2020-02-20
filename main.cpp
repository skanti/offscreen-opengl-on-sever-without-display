#include <cmath>
#include <random>
#include <chrono>
#include <unordered_map>

#include <omp.h>
#include <eigen3/Eigen/Dense>

#define IS_GLX 1 // <-- this is the of offscreen flag

#define WINDOW_HEIGHT 960
#define WINDOW_WIDTH 1280 

#include "opengl/OpenGLHelper.h"
#include "opengl/CameraHelper.h"

#include "args.hxx"

#if IS_GLX == 1
#include "opengl/GLXHelper.h"
#endif 

#include "opengl/Types.h"
#include "geometry/Box3D.h"

#include "vao/VoxelGrid.h"
#include "vao/Harris3D.h"

#include "LoaderVOX.h"
#include "Types.h"





struct InputArgs {
	std::string vox;
	std::string out;
} inargs;

struct VoxelGridData {
	Eigen::MatrixXf vertices, normals, positions, colors;
	float res;
};

class ToyWorld {
public:

    void term() {
        
    }
	
	void load_data() {
		load_vox<float, 1>(inargs.vox, vox.grid_dims, vox.res, vox.grid2world, vox.sdf);
		vox.pdf.resize(vox.sdf.size());

	}

	void init() {
		init_mvp(); // <-- initializes model-view-projection matrix
		load_data(); // <-- loads sdf grid data

		run(); // <-- launches 3D harris keypoint detection algorithm
		
		create_position_and_color(); // <-- extracts detected keypoints

		// -> from here not so important as it only visualizes the detected voxels
		ct::geometry::Box3D::create(voxelgrid.vertices, voxelgrid.normals); 
		ct::vao::VoxelGrid::init_vao(vao_grid, voxelgrid.vertices, voxelgrid.normals, voxelgrid.positions, voxelgrid.colors, voxelgrid.res, std::string(HOME_DIR) + "./vao/");
		// <-
	}

	void run() {
		cso::Harris3D::init(cso_harris3d, std::string(HOME_DIR) + "./vao/");
		setup_gpu_memory();
		cso::Harris3D::run(cso_harris3d, vox.grid_dims, vox.res, tex_sdf, tex_pdf, tex_pdf_nms);
		download_image3d();
		//clean();
		save_csv();
		//save_vox<float, 1, float, 1>("tmp.vox2", vox.grid_dims, vox.res, vox.grid2world, vox.sdf, vox.pdf);
	}

	void download_image3d() {
		glActiveTexture(GL_TEXTURE0 + tex_pdf_nms.unit);
		glBindTexture(GL_TEXTURE_3D, tex_pdf_nms.id);
		glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, vox.pdf.data());
		glBindBuffer(GL_TEXTURE_3D, 0);
	}

	void setup_gpu_memory() {
		// -> pdf tex
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		tex_sdf.unit = 0;
		glActiveTexture(GL_TEXTURE0 + tex_sdf.unit);
		glGenTextures(1, &tex_sdf.id);
		glBindTexture(GL_TEXTURE_3D, tex_sdf.id);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, vox.grid_dims(0), vox.grid_dims(1), vox.grid_dims(2));
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, vox.grid_dims(0), vox.grid_dims(1), vox.grid_dims(2), GL_RED, GL_FLOAT, vox.sdf.data());
		glBindTexture(GL_TEXTURE_3D, 0);
		// <-
		
		// -> pdf tex
		tex_pdf.unit = 1;
		glActiveTexture(GL_TEXTURE0 + tex_pdf.unit);
		glGenTextures(1, &tex_pdf.id);
		glBindTexture(GL_TEXTURE_3D, tex_pdf.id);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, vox.grid_dims(0), vox.grid_dims(1), vox.grid_dims(2));
		//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, vox.grid_dims(0), vox.grid_dims(1), vox.grid_dims(2), GL_RED, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_3D, 0);
		// <-
		
		// -> pdf nms tex
		tex_pdf_nms.unit = 2;
		glActiveTexture(GL_TEXTURE0 + tex_pdf_nms.unit);
		glGenTextures(1, &tex_pdf_nms.id);
		glBindTexture(GL_TEXTURE_3D, tex_pdf_nms.id);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, vox.grid_dims(0), vox.grid_dims(1), vox.grid_dims(2));
		//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, vox.grid_dims(0), vox.grid_dims(1), vox.grid_dims(2), GL_RED, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_3D, 0);
		// <-

	}

	void save_csv() {
		std::ofstream file;
		file.open(inargs.out);
		file << "# (p_scan vec3f) px, py, pz" << std::endl;
		for (int k = 0; k < vox.grid_dims(2); k++) {
			for (int j = 0; j < vox.grid_dims(1); j++) {
				for (int i = 0; i < vox.grid_dims(0); i++) {
					int index = k*vox.grid_dims(1)*vox.grid_dims(0) + j*vox.grid_dims(0) + i;
					if (std::abs(vox.sdf[index]) <= 1*vox.res && vox.pdf[index]) {
						Eigen::Vector3f p = (vox.grid2world*Eigen::Vector4f(i, j, k, 1)).topRows(3);
						file << p(0) << "," << p(1) << "," << p(2) << std::endl;
					}
				}	
			}	
		}

		file.close();
	}

	void rotate_model_matrix() {
		Eigen::Matrix4f rotx = Eigen::Matrix4f::Identity(); 
		rotx.block(0, 0, 3, 3) = (Eigen::Matrix3f)Eigen::AngleAxisf(-M_PI*0.5, Eigen::Vector3f::UnitX());
	}

	void clean() {
		for (int k = 0; k < vox.grid_dims(2); k++) {
			for (int j = 0; j < vox.grid_dims(1); j++) {
				for (int i = 0; i < vox.grid_dims(0); i++) {
					int index = k*vox.grid_dims(1)*vox.grid_dims(0) + j*vox.grid_dims(0) + i;
					if (std::abs(vox.sdf[index]) > 1*vox.res) {
						vox.pdf[index] = 0;
					}
				}
			}
		}
	}

	void create_position_and_color() {
		std::vector<float> positions, colors;
		n_size = 0;
		int counter = 0;
		for (int k = 0; k < vox.grid_dims(2); k++) {
			for (int j = 0; j < vox.grid_dims(1); j++) {
				for (int i = 0; i < vox.grid_dims(0); i++) {
					int index = k*vox.grid_dims(1)*vox.grid_dims(0) + j*vox.grid_dims(0) + i;
					if (std::abs(vox.sdf[index]) <= 1*vox.res) {
						Eigen::Vector3f p = (vox.grid2world*Eigen::Vector4f(i, j, k, 1)).topRows(3);
						positions.push_back(p(0));
						positions.push_back(p(1));
						positions.push_back(p(2));
						counter += (vox.pdf[index] > 0);
						colors.push_back(vox.pdf[index]);
						colors.push_back(0.2);
						colors.push_back(0.2);
							//printf("hit: %d %d %d\n", i, j, k);
						n_size++;

					}
				}	
			}	
		}

		printf("n_size: %d\n", n_size);
		printf("n_counter_harris: %d\n", counter);
		voxelgrid.positions = Eigen::Map<Eigen::MatrixXf> (positions.data(), 3, n_size);
		voxelgrid.colors = Eigen::Map<Eigen::MatrixXf> (colors.data(), 3, n_size);

		vox.res = 0.45;
		//printf("res %f\n", res);
	}

	void init_mvp() {
		model_matrix.setIdentity();
		float ar = (float)(WINDOW_WIDTH)/WINDOW_HEIGHT;
		Eigen::Vector3f eye(0, 0, -1);
		Eigen::Vector3f up(0, 1, 0);
		Eigen::Vector3f lookat(0, 0, 0);
		view_matrix = ct::opengl::make_view_matrix<Eigen::Vector3f>(eye, lookat, up);	
		projection_matrix = ct::opengl::make_perspective_matrix<float>(60.0f, ar, 0.03f, 100.0f); 	
		//projection_matrix = ct::opengl::make_ortho_matrix<float>(-1, 1, -1, 1, 0.3, 50); 	
	}
	
	void draw() {
		ct::vao::VoxelGrid::draw(vao_grid, n_size, model_matrix, view_matrix, projection_matrix);
	}

	
	void update_camera() {
		Eigen::Map<Eigen::Vector3f> eye(Camera::cam_position);
		Eigen::Map<Eigen::Vector3f> up(Camera::cam_up);
		Eigen::Map<Eigen::Vector3f> lookat(Camera::cam_lookat);
		view_matrix = ct::opengl::make_view_matrix<Eigen::Vector3f>(eye, lookat, up);	
		Camera::trans_rate = 0.01;

	}

    void advance(std::size_t iteration_counter, int64_t ms_per_frame) {
		update_camera();
    }

private:
	Eigen::Matrix4f model_matrix;
	Eigen::Matrix4f projection_matrix;
	Eigen::Matrix4f view_matrix;
	
	ct::opengl::Texture tex_sdf, tex_pdf, tex_pdf_nms;
	VoxelGridData voxelgrid;
	cso::Harris3D::CSO cso_harris3d;
	Vox vox;

	ct::vao::VoxelGrid::VAO vao_grid;
	
	int n_size;
};

void parse_args(int argc, char** argv) {

	args::ArgumentParser parser("This is a test program.", "This goes after the options.");
	args::Group allgroup(parser, "", args::Group::Validators::All);

	args::ValueFlag<std::string> vox(allgroup, "bunny.vox", "vox file", {"vox"});
	args::ValueFlag<std::string> out(allgroup, "bunny.csv", "csv file", {"out"});
	
	try {
        parser.ParseCLI(argc, argv);
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
		exit(1);
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
		exit(1);
    }

	inargs.vox = args::get(vox);
	inargs.out = args::get(out);

};

int main(int argc, char** argv) {
	parse_args(argc, argv);
#if IS_GLX == 0
	printf("on-screen (rendering) mode...\n\n");
	GLFWwindow *window;
	ct::opengl::init_gl("Voxelization Viewer", WINDOW_WIDTH, WINDOW_HEIGHT, &window);
	glfwSetCursorPosCallback(window, Camera::mousemove_glfwCursorPosCallback);
	glfwSetScrollCallback(window, Camera::mousemove_glfwScrollCallback);
	glfwSetMouseButtonCallback(window, Camera::mousemove_glfwMouseButtonCallback);
    //ct::opengl::set_dummy_key_callback(window);
    ToyWorld world;
	world.init();
	
    std::chrono::high_resolution_clock clock;
    std::size_t iteration_counter = 0;
    std::chrono::high_resolution_clock::time_point t0 = clock.now();
    std::chrono::milliseconds mspf;
    while (ct::opengl::is_window_alive(window)) {
		ct::opengl::poll_events();
		ct::opengl::clear_screen();

        world.advance(iteration_counter++, mspf.count());
        world.draw();
	
        mspf = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - t0);
        t0 = clock.now();

		ct::opengl::swap_window_buffer(window);
    }
	world.term();
	ct::opengl::terminate_window();	
#else 
	printf("off-screen mode...\n\n");
	ct::opengl::create_glx_context();
    ToyWorld world;
	world.init();

#endif

    return 0;
}
