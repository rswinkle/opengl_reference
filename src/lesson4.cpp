#include <gltools.h>
#include <rsw_matstack.h>


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




int main(int argc, char** argv)
{
	setup_context();

	float pyramid_verts[] = {
		// Front face
		 0.0,  1.0,  0.0,
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		// Right face
		 0.0,  1.0,  0.0,
		 1.0, -1.0,  1.0,
		 1.0, -1.0, -1.0,
		// Back face
		 0.0,  1.0,  0.0,
		 1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		// Left face
		 0.0,  1.0,  0.0,
		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0
	};

	float pyramid_colors[] = {
		// Front face
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		// Right face
		1.0, 0.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		// Back face
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		// Left face
		1.0, 0.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 1.0, 0.0, 1.0
	};

	float cube_verts[] = {
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

	float cube_colors[] = {
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,

		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,

		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,

		1.0, 0.5, 0.5, 1.0,
		1.0, 0.5, 0.5, 1.0,
		1.0, 0.5, 0.5, 1.0,
		1.0, 0.5, 0.5, 1.0,

		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,

		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0, 
		0.0, 0.0, 1.0, 1.0 
	};

	unsigned short cube_triangles[] = {
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

	GLuint pyramid_buf, pyramid_color_buf;
	glGenBuffers(1, &pyramid_buf);
	glGenBuffers(1, &pyramid_color_buf);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_verts), pyramid_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_color_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_colors), pyramid_colors, GL_STATIC_DRAW);

	GLuint cube_buf, cube_color_buf;
	glGenBuffers(1, &cube_buf);
	glGenBuffers(1, &cube_color_buf);
	glBindBuffer(GL_ARRAY_BUFFER, cube_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, cube_color_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

	GLuint cube_tri_buf;
	glGenBuffers(1, &cube_tri_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_tri_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangles), cube_triangles, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	const char vs_file[] = "../media/shaders/transform_vertcolors.vp";
	const char fs_file[] = "../media/shaders/smooth_color.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(program);


	matrix_stack mat_stack;
	rsw::make_perspective_matrix(mat_stack.stack[mat_stack.top], DEG_TO_RAD(45), WIDTH/(float)HEIGHT, 0.1f, 100.0f);


	float r_pyramid = 0, r_cube = 0;
	float elapsed;


	mat4 mvp_mat;

	int mvp_loc = glGetUniformLocation(program, "mvp_mat");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	unsigned int last_time = SDL_GetTicks();
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

		elapsed = new_time - last_time;
		last_time = new_time;

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		r_pyramid += 90 * (elapsed / 1000.0f);
		r_cube -= 75 * (elapsed / 1000.0f);

		mat_stack.push();
		mat_stack.translate(-1.5, 0, -7.0);
		mat_stack.push();

		mat_stack.rotate(DEG_TO_RAD(r_pyramid), 0, 1, 0);

		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mat_stack.stack[mat_stack.top]);
		glBindBuffer(GL_ARRAY_BUFFER, pyramid_buf);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, pyramid_color_buf);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		mat_stack.pop();

		mat_stack.translate(3.0, 0.0, 0.0);
		mat_stack.rotate(DEG_TO_RAD(r_cube), 1, 1, 1);

		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mat_stack.stack[mat_stack.top]);
		glBindBuffer(GL_ARRAY_BUFFER, cube_buf);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, cube_color_buf);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawElements(GL_TRIANGLES, sizeof(cube_triangles), GL_UNSIGNED_SHORT, 0);

		mat_stack.pop();

		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &pyramid_buf);
	glDeleteBuffers(1, &pyramid_color_buf);
	glDeleteBuffers(1, &cube_buf);
	glDeleteBuffers(1, &cube_color_buf);
	glDeleteBuffers(1, &cube_tri_buf);
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

	window = SDL_CreateWindow("Lesson 4", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
			}
		}
	}
	return 0;
}








