
#include "glad_gles2.h"
#include <SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>


#include "gltools.h"


#define WIDTH 640
#define HEIGHT 480

SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events();

int main()
{
	int frames;
	setup_context();

	float points[] = { -0.5, -0.5, 0,
	                    0.5, -0.5, 0,
	                    0,    0.5, 0 };

	//no error checking done for any of this except shader compilation
	GLuint program = load_shader_file_pair("../../media/shaders/passthrough_gles.vs", "../../media/shaders/uniform_color_gles.fs");
	if (!program) {
		printf("failed to compile/link shaders\n");
		check_errors(0, "Checking for errors after shaders");
		cleanup();
	}

	glUseProgram(program);

	float Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	set_uniform4fv(program, "color", Red);

	//no vaos in GLES 2 without extensions (OES suffix), but no need for one either
	//GLuint vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	//GLuint triangle;
	//glGenBuffers(1, &triangle);
	//glBindBuffer(GL_ARRAY_BUFFER, triangle);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	// no need for a buffer at all, can use client arrays
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, points);

	frames = 0;
	int start = SDL_GetTicks();
	int interval = 300;
	int old_time = start, new_time = start;
	int old_frames = 0;

	while (1) {
		if (handle_events()) {
			break;
		}
		
		new_time = SDL_GetTicks();
		if (new_time - old_time >= 1000) {
			printf("%.2f FPS\n", 1000.0f*(frames-old_frames)/(new_time-old_time));
			old_time = new_time;
			old_frames = frames;
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
		
		frames++;
	}
	int stop = SDL_GetTicks();
	float time = (stop-start)/1000.0f;
	
	printf("total frames = %d\n", frames);
	printf("total time (s) = %.2f\n", time);
	printf("Avg FPS: %.2f\n", frames/time);

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

int handle_events()
{
	SDL_Event e;
	int sc;
	int w, h;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			return 1;
		} else if (e.type == SDL_KEYDOWN) {
			sc = e.key.keysym.scancode;

			if (sc == SDL_SCANCODE_ESCAPE) {
				return 1;
			}
		} if (e.type == SDL_WINDOWEVENT) {
			if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				w = e.window.data1;
				h = e.window.data2;
				glViewport(0, 0, w, h);
			}
		}
	}
	return 0;
}

void setup_context()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	window = SDL_CreateWindow("GLES Hello Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		printf("Failed to create window: %s\n", SDL_GetError());
		cleanup();
	}

	glcontext = SDL_GL_CreateContext(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress)) {
		puts("GLAD init failed");
		cleanup();
	}

	check_errors(0, "Checking for errors after setup");

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
	exit(0);
}
