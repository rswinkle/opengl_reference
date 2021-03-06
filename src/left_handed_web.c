#include <GL/glew.h>

//included in SDL.h?
//#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define WIDTH 640
#define HEIGHT 480

//compile with this command
//gcc -std=c99 left_handed.c -o left_handed -lSDL2 -lGLEW -lGL
//emcc left_handed_web.c -O2 --emrun -s WASM=1 -s USE_SDL=2 -o left_handed_web.html

float Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

SDL_Window* window;
SDL_GLContext glcontext;

int depth_test;
int uniform_loc;

void cleanup();
void setup_context();
int handle_events();


void check_errors(int n, const char* str);
int compile_shader_str(GLuint shader, const char* shader_str);
int link_program(GLuint program);
GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src);

void main_loop_iter();


static const char vs_shader_str[] =
"attribute vec4 vertex;                \n"
"void main(void)                       \n"
"{\n"
"	gl_Position = vertex;\n"
"}";

static const char fs_shader_str[] =
"precision mediump float;              \n"
"uniform vec4 color;                   \n"
"void main(void)                       \n"
"{                                     \n"
"	gl_FragColor = color;              \n"
"}";

int main(int argc, char** argv)
{
	setup_context();

#define NUM_TRIANGLES 2

	float points[] = { -1,  1,  -0.5,
	                   -1, -1,  -0.5,
	                    1, -1,  -0.5,

	                    1,  1,   0.5,
	                   -1, -1,   0.5,
	                    1, -1,   0.5 };

	//no error checking done for any of this except shader compilation
	GLuint program = load_shader_pair(vs_shader_str, fs_shader_str);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);

	uniform_loc = glGetUniformLocation(program, "color");
	if (uniform_loc < 0) {
		printf("Uniform: v_color not found.\n");
		exit(0);
	}
	glUniform4fv(uniform_loc, 1, Red);

	glClearColor(0, 0, 0, 1);
	depth_test = 1;
	glEnable(GL_DEPTH_TEST);


	puts("Red triangle is at -0.5 z");
	puts("Blue triangle is at 0.5 z");
	puts("Depth test ON");
	puts("Ergo, positive z is into the screen and OpenGL is left-handed!");
	puts("Press D to toggle depth test...");

	//no default vao in core profile ...
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint triangle;
	glGenBuffers(1, &triangle);
	glBindBuffer(GL_ARRAY_BUFFER, triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);


	// does single attrib default to location 0 so could skip getAttribLoc?
	int vert_loc = glGetAttribLocation(program, "vertex");
	glEnableVertexAttribArray(vert_loc);
	glVertexAttribPointer(vert_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);



#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop_iter, 0, 1);
#else
	while (1) {
		main_loop_iter();
	}
#endif

	glDeleteBuffers(1, &triangle);
	glDeleteProgram(program);

	cleanup();

	return 0;
}

void main_loop_iter()
{
	if (handle_events()) {
		cleanup();
		//break;
	}

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform4fv(uniform_loc, 1, Red);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glUniform4fv(uniform_loc, 1, Blue);
	glDrawArrays(GL_TRIANGLES, 3, 3);

	SDL_GL_SwapWindow(window);
}



void setup_context()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("OpenGL is left handed!", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
	exit(0);
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
			} else if (sc == SDL_SCANCODE_D) {
				if (depth_test) {
					glDisable(GL_DEPTH_TEST);
					puts("Depth test OFF");
					puts("Press D to toggle depth test...");
				} else {
					glEnable(GL_DEPTH_TEST);
					puts("Depth test ON");
					puts("Red triangle is at -0.5 z");
					puts("Blue triangle is at 0.5 z");
					puts("Depth test ON");
					puts("Ergo, OpenGL is left-handed!");
					puts("Press D to toggle depth test...");
				}
				depth_test = !depth_test;
			}
		}
	}
	return 0;
}








void check_errors(int n, const char* str)
{
	GLenum error;
	int err = 0;
	while ((error = glGetError()) != GL_NO_ERROR) {
		switch (error)
		{
		case GL_INVALID_ENUM:
			fprintf(stderr, "invalid enum\n");
			break;
		case GL_INVALID_VALUE:
			fprintf(stderr, "invalid value\n");
			break;
		case GL_INVALID_OPERATION:
			fprintf(stderr, "invalid operation\n");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			fprintf(stderr, "invalid framebuffer operation\n");
			break;
		case GL_OUT_OF_MEMORY:
			fprintf(stderr, "out of memory\n");
			break;
		default:
			fprintf(stderr, "wtf?\n");
		}
		err = 1;
	}
	if (err)
		fprintf(stderr, "%d: %s\n\n", n, (!str)? "Errors cleared" : str);
}

#define BUF_SIZE 1000

int link_program(GLuint program)
{
	glLinkProgram(program);
	int status = 0;
	char info_buf[BUF_SIZE];
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		int len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		if (len > 0) {
			int written = 0;
			glGetProgramInfoLog(program, len, &written, info_buf);
			printf("Link failed:\n===============\n%s\n", info_buf);
		}
		return 0;
	}

	return program;
}

int compile_shader_str(GLuint shader, const char* shader_str)
{
	glShaderSource(shader, 1, &shader_str, NULL);
	glCompileShader(shader);

	int result;
	char shader_info_buf[BUF_SIZE];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		int length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length > 0 && length <= BUF_SIZE) {
			int written = 0;
			glGetShaderInfoLog(shader, BUF_SIZE, &written, shader_info_buf);

			printf("Compile failed:\n===============\n%s\n", shader_info_buf);
		}
		return 0;
	}
	return 1;
}

GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src)
{
	GLuint program, vert_shader, frag_shader;

	program = glCreateProgram();
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	if (!compile_shader_str(vert_shader, vert_shader_src))
		return 0;
	if (!compile_shader_str(frag_shader, frag_shader_src))
		return 0;

	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return link_program(program);
}

