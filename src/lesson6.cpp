#include <gltools.h>


#include <SDL2/SDL.h>

#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

using namespace std;

using rsw::vec3;
using rsw::mat4;


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events();




int polygon_mode;
int cur_tex;
GLuint textures[3];

float z;
mat4 proj_mat;
mat4 vp_mat;

int mvp_loc;

int main(int argc, char** argv)
{
	setup_context();

	polygon_mode = 2;


	glGenTextures(3, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	if (!load_texture2D("../media/textures/crate.gif", GL_NEAREST, GL_NEAREST, GL_MIRRORED_REPEAT, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	if (!load_texture2D("../media/textures/crate.gif", GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	if (!load_texture2D("../media/textures/crate.gif", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_MIRRORED_REPEAT, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, textures[0]);

	float vertices[] = {
		// Front face
		-1.0, -1.0,  1.0,
 	 	 1.0, -1.0,  1.0,
 	 	 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// Back face
		-1.0, -1.0, -1.0,
		-1.0,  1.0, -1.0,
 	 	 1.0,  1.0, -1.0,
 	 	 1.0, -1.0, -1.0,
		// Top face
		-1.0,  1.0, -1.0,
		-1.0,  1.0,  1.0,
 	 	 1.0,  1.0,  1.0,
 	 	 1.0,  1.0, -1.0,
		// Bottom face
		-1.0, -1.0, -1.0,
 	 	 1.0, -1.0, -1.0,
 	 	 1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
		// Right face
 	 	 1.0, -1.0, -1.0,
 	 	 1.0,  1.0, -1.0,
 	 	 1.0,  1.0,  1.0,
 	 	 1.0, -1.0,  1.0,
		// Left face
		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0
	};

	float texcoords[] = {
		// Front face
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// Back face
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		// Top face
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		// Bottom face
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		// Right face
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		// Left face
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0
	};

	GLuint triangles[] = {
		0, 1, 2,      0, 2, 3,    // Front face
		4, 5, 6,      4, 6, 7,    // Back face
		8, 9, 10,     8, 10, 11,  // Top face
		12, 13, 14,   12, 14, 15, // Bottom face
		16, 17, 18,   16, 18, 19, // Right face
		20, 21, 22,   20, 22, 23  // Left face
	};


	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vert_buf;
	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tex_buf;
	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint elem_buf;
	glGenBuffers(1, &elem_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	const char vs_file[] = "../media/shaders/texturing.vp";
	const char fs_file[] = "../media/shaders/texturing.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(program);


	rsw::make_perspective_matrix(proj_mat, DEG_TO_RAD(45), WIDTH/(float)HEIGHT, 0.1f, 100.0f);

	z = -5;
	vp_mat = proj_mat * rsw::translation_mat4(0, 0, z);

	mat4 mvp_mat;
	mat4 rot_mat(1);

	mvp_loc = glGetUniformLocation(program, "mvp_mat");


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	unsigned int old_time = 0, new_time=0, counter = 0;
	while (1) {
		if (handle_events())
			break;

		counter++;
		new_time = SDL_GetTicks();
		if (new_time - old_time > 3000) {
			printf("%f FPS\n", counter*1000.f/(new_time-old_time));
			old_time = new_time;
			counter = 0;
		}

		rsw::load_rotation_mat4(rot_mat, vec3(0, 1, 0), 0.5f*new_time/1000.0f);

		mvp_mat = vp_mat * rot_mat;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mvp_mat);
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof(triangles), GL_UNSIGNED_INT, 0);


		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vert_buf);
	glDeleteBuffers(1, &tex_buf);
	glDeleteBuffers(1, &elem_buf);
	glDeleteProgram(program);

	cleanup();

	return 0;
}


void setup_context()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("Ex 3", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		cleanup();
		exit(0);
	}

	glcontext = SDL_GL_CreateContext(window);
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		printf("Error: %s\n", glewGetErrorString(err));
		cleanup();
		exit(0);
	}

	check_errors(0, "Clearing stupid error after glewInit");

	int major, minor, profile;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

	printf("OpenGL version %d.%d with profile %d\n", major, minor, profile);
}

void cleanup()
{
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

int handle_events()
{
	SDL_Event e;
	int sc;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			return 1;
		} else if (e.type == SDL_KEYDOWN) {
			sc = e.key.keysym.scancode;

			if (sc == SDL_SCANCODE_ESCAPE) {
				return 1;
			} else if (sc == SDL_SCANCODE_P) {
				polygon_mode = (polygon_mode + 1) % 3;
				if (polygon_mode == 0)
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				else if (polygon_mode == 1)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			} else if (sc == SDL_SCANCODE_F) {
				
				cur_tex = (cur_tex + 1) % 3;
				if (cur_tex == 0) {
					puts("GL_NEAREST\n");
				} else if (cur_tex == 1 ) {
					puts("GL_LINEAR\n");
				} else {
					puts("GL_LINEAR_MIPMAP_NEAREST");
				}
				glBindTexture(GL_TEXTURE_2D, textures[cur_tex]);

			// if I decide to let the user control rotation
			} else if (sc == SDL_SCANCODE_LEFT) {

			} else if (sc == SDL_SCANCODE_RIGHT) {

			} else if (sc == SDL_SCANCODE_UP) {

			} else if (sc == SDL_SCANCODE_DOWN) {

			} else if (sc == SDL_SCANCODE_EQUALS) {
				z += 0.05;
				vp_mat = proj_mat * rsw::translation_mat4(0, 0, z);
			} else if (sc == SDL_SCANCODE_MINUS) {
				z -= 0.05;
				vp_mat = proj_mat * rsw::translation_mat4(0, 0, z);
			}
		}
	}
	return 0;
}




