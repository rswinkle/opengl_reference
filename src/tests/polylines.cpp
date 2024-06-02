#include <GL/glew.h>

#include <SDL2/SDL.h>

#include "gltools.h"

#include "rsw_math.h"

#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480


using rsw::mat4;

SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events();


float line_width;
int aliased;
int blending = 1;
float granularity;
int hint_nicest;

int main(int argc, char** argv)
{
	setup_context();

	line_width = 1;
	float w2 = line_width/2.0f;
	aliased = 1;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	float polyline[] =
	{
		50, 270, 0,     1, 0, 0, 0,
		100, 30, 0,     1, 0, 0, 0,

		50+w2, 270, 0,     1, 0, 0, 1,
		100+w2, 30, 0,     1, 0, 0, 1,

		50+line_width, 270, 0,     1, 0, 0, 0,
		100+line_width, 30, 0,     1, 0, 0, 0,


		1, 1, 0,        0, 0, 0, 0
	};

	//no error checking done for any of this except shader compilation
	GLuint program = load_shader_file_pair("../media/shaders/transform_vertcolors.vp", "../media/shaders/smooth_color.fp");
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);

	int mvp_loc = glGetUniformLocation(program, "mvp_mat");

	mat4 mvp_mat;
	rsw::make_orthographic_matrix(mvp_mat, 0, WIDTH-1, HEIGHT-1, 0, 1, -1);
	//rsw::make_orthographic_matrix(mvp_mat, 0, WIDTH-1, 0, HEIGHT-1, 1, -1);

	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mvp_mat);
	

	//no default vao in core profile ...
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint line_buf;
	glGenBuffers(1, &line_buf);
	glBindBuffer(GL_ARRAY_BUFFER, line_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(polyline), polyline, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*7, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)(sizeof(float)*3));

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glPointSize(10);

	unsigned int old_time = 0, new_time=0, counter = 0;
	while (1) {
		if (handle_events())
			break;

		counter++;
		new_time = SDL_GetTicks();
		if (new_time - old_time > 3000) {
			printf("%f FPS\n", counter*1000.f/(new_time-old_time));
			fflush(stdout); //stupid windows doesn't flush with \n >:-/
			old_time = new_time;
			counter = 0;
		}

		
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

		SDL_GL_SwapWindow(window);
	}

	glDeleteBuffers(1, &line_buf);
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

	window = SDL_CreateWindow("Test Polyline", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

			switch (sc) {
			case SDL_SCANCODE_ESCAPE:
				return 1;
				break;
			case SDL_SCANCODE_UP:
				line_width += granularity;
				glLineWidth(line_width);
				printf("line_width = %f\n", line_width);
				break;
			case SDL_SCANCODE_DOWN:
				line_width -= granularity;
				if (line_width < 1.0) {
					line_width = 1.0f;
				}
				glLineWidth(line_width);
				printf("line_width = %f\n", line_width);
				break;
			case SDL_SCANCODE_SPACE:
				aliased = !aliased;
				if (!aliased) {
					glEnable(GL_LINE_SMOOTH);
					puts("smooth");
				} else {
					glDisable(GL_LINE_SMOOTH);
					puts("aliased");
				}
				break;
			case SDL_SCANCODE_B:
				blending = !blending;

				if (blending) {
					glEnable(GL_BLEND);
					puts("blending");
				} else {
					glDisable(GL_BLEND);
					puts("no blending");
				}
				break;
			case SDL_SCANCODE_H:
				hint_nicest = !hint_nicest;
				if (hint_nicest) {
					puts("nicest");
					glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
				} else {
					puts("fastest");
					glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
				}
				break;
			}
			
		}
	}
	return 0;
}









