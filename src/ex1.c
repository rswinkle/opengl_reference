#include <GL/glew.h>

//included in SDL.h?
//#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events();


void check_errors(int n, const char* str);
int compile_shader_str(GLuint shader, const char* shader_str);
int link_program(GLuint program);
GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src);



static const char vs_shader_str[] =
"#version 330 core                     \n"
"                                      \n"
"layout (location = 0) in vec4 vertex; \n"
"void main(void)                       \n"
"{\n"
"	gl_Position = vertex;\n"
"}";

static const char fs_shader_str[] =
"#version 330 core                     \n"
"                                      \n"
"uniform vec4 color;                   \n"
"out vec4 frag_color;                  \n"
"void main(void)                       \n"
"{                                     \n"
"	frag_color = color;                \n"
"}";

int main(int argc, char** argv)
{
	setup_context();

	float points[] = { -0.5, -0.5, 0,
	                    0.5, -0.5, 0,
	                    0,    0.5, 0 };

	//no error checking done for any of this except shader compilation
	GLuint program = load_shader_pair(vs_shader_str, fs_shader_str);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);

	float Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	int loc = glGetUniformLocation(program, "color");
	glUniform4fv(loc, 1, Red);

	//no default vao in core profile ...
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint triangle;
	glGenBuffers(1, &triangle);
	glBindBuffer(GL_ARRAY_BUFFER, triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);



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

	window = SDL_CreateWindow("Ex 1", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
