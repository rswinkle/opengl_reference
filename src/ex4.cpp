#include <gltools.h>


#include <SDL2/SDL.h>

#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events();




static const char vs_shader_str[] =
"#version 330 core                        \n"
"                                         \n"
"layout (location = 0) in vec4 in_vertex; \n"
"layout (location = 1) in vec4 in_color;  \n"
"out vec4 vary_color;                     \n"
"void main(void)                          \n"
"{                                        \n"
"	vary_color = in_color;                \n"
"	gl_Position = in_vertex;              \n"
"}";

static const char fs_shader_str[] =
"#version 330 core                     \n"
"                                      \n"
"in vec4 vary_color;                   \n"
"out vec4 frag_color;                  \n"
"void main(void)                       \n"
"{                                     \n"
"	frag_color = vary_color;           \n"
"}";

int main()
{
	setup_context();

	float points_n_colors[] = {
		-0.5, -0.5, 0.0,
		 1.0,  0.0, 0.0,

		 0.5, -0.5, 0.0,
		 0.0,  1.0, 0.0,

		 0.0,  0.5, 0.0,
		 0.0,  0.0, 1.0 };


	//no error checking done for any of this except shader compilation
	GLuint program = load_shader_pair(vs_shader_str, fs_shader_str);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint triangle;
	glGenBuffers(1, &triangle);
	glBindBuffer(GL_ARRAY_BUFFER, triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_n_colors), points_n_colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));


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

		
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}

	glDeleteBuffers(1, &triangle);
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

	window = SDL_CreateWindow("Ex 2", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

			if (sc == SDL_SCANCODE_ESCAPE)
				return 1;
		}
	}
	return 0;
}












