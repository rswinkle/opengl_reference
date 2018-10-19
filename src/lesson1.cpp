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

	float triangle_verts[] = {
		 0.0,  1.0,  0.0,
		-1.0, -1.0,  0.0,
		 1.0, -1.0,  0.0
	};

	float square_verts[] = {
		 1.0,  1.0,  0.0,
		-1.0,  1.0,  0.0,
		 1.0, -1.0,  0.0,
		-1.0, -1.0,  0.0
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint tri_buf;
	glGenBuffers(1, &tri_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tri_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_verts), triangle_verts, GL_STATIC_DRAW);

	GLuint square_buf;
	glGenBuffers(1, &square_buf);
	glBindBuffer(GL_ARRAY_BUFFER, square_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_verts), square_verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	const char vs_file[] = "../media/shaders/basic_transform.vp";
	const char fs_file[] = "../media/shaders/white.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(program);


	matrix_stack mat_stack;
	rsw::make_perspective_matrix(mat_stack.stack[mat_stack.top], DEG_TO_RAD(45), WIDTH/(float)HEIGHT, 0.1f, 100.0f);


	//mat_stack.load_mat(proj_mat);

	mat4 mvp_mat;

	int mvp_loc = glGetUniformLocation(program, "mvp_mat");

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

		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


		mat_stack.push();
		mat_stack.translate(-1.5, 0, -7.0);

		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mat_stack.stack[mat_stack.top]);
		glBindBuffer(GL_ARRAY_BUFFER, tri_buf);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		mat_stack.translate(3.0, 0.0, 0.0);

		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mat_stack.stack[mat_stack.top]);
		glBindBuffer(GL_ARRAY_BUFFER, square_buf);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		mat_stack.pop();

		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &tri_buf);
	glDeleteBuffers(1, &square_buf);
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

	window = SDL_CreateWindow("Lesson 1", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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





