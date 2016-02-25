#include <GL/glew.h>
#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <stdio.h>


#define WIDTH 640
#define HEIGHT 480

//compile with this command
//gcc -std=c99 left_handed.c -o left_handed -lSDL2 -lGLEW -lGL

float Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

SDL_Window* window;
SDL_GLContext glcontext;

int depth_test;

void cleanup();
void setup_context();
int handle_events();


int link_program(GLuint program);
int compile_shader_str(GLuint shader, const char* shader_str);
GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src);


int main()
{
	setup_context();

#define NUM_TRIANGLES 2

	float points[] = { -1,  1,  -0.5,
	                   -1, -1,  -0.5,
	                    1, -1,  -0.5,

	                    1,  1,   0.5,
	                   -1, -1,   0.5,
	                    1, -1,   0.5 };


const char vertex_shader[] = "#version 130\n"
"in vec4 vertex;\n"
"void main(void)\n"
"{\n"
"	// No transforming, no confusion, just pass it straight in\n"
"	gl_Position = vertex;\n"
"}\n";

const char frag_shader[] = "#version 130\n"
"uniform vec4 color;\n"
"out vec4 fragcolor;\n"
"void main(void)\n"
"{\n"
"	fragcolor = color;\n"
"}\n";





	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	GLuint program = load_shader_pair(vertex_shader, frag_shader);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);
	int uniform_loc = glGetUniformLocation(program, "color");
	if (uniform_loc < 0) {
		printf("Uniform: v_color not found.\n");
		exit(0);
	}

	glClearColor(0, 0, 0, 1);
	depth_test = 1;
	glEnable(GL_DEPTH_TEST);


	printf("Red triangle is at -0.5 z\n");
	printf("Blue triangle is at 0.5 z\n");
	printf("Depth test ON\n");

	while (1) {
		if (handle_events())
			break;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform4fv(uniform_loc, 1, Red);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUniform4fv(uniform_loc, 1, Blue);
		glDrawArrays(GL_TRIANGLES, 3, 3);

		SDL_GL_SwapWindow(window);
	}

	cleanup();

	return 0;
}


void setup_context()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(0);
	}

	window = SDL_CreateWindow("OpenGL is left handed!", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		cleanup();
		exit(0);
	}

	glcontext = SDL_GL_CreateContext(window);
	
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		printf("Error: %s\n", glewGetErrorString(err));
		cleanup();
		exit(0);
	}
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
			} else if (sc == SDL_SCANCODE_D) {
				if (depth_test) {
					glDisable(GL_DEPTH_TEST);
					printf("Depth test OFF\n");
				} else {
					glEnable(GL_DEPTH_TEST);
					printf("Depth test ON\n");
				}
				depth_test = !depth_test;
			}
		}
	}
	return 0;
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

	return 1;
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

