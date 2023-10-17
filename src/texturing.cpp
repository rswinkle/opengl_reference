#include <gltools.h>

#include <rsw_math.h>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>

#define WIDTH 640
#define HEIGHT 480

using rsw::mat4;
using rsw::vec3;



SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events(unsigned int last_time, unsigned int cur_time, mat4& mvp_mat);


#define NUM_TEXTURES 3
GLuint textures[NUM_TEXTURES];
int tex_index;
int tex_filter;




int main(int argc, char** argv)
{
	setup_context();

	float points_n_tex[] =
	{
		-0.5,  0.5, -0.1,
		-0.5, -0.5, -0.1,
		 0.5,  0.5, -0.1,
		 0.5, -0.5, -0.1,

		 0.0, 0.0,
		 0.0, 1.0,
		 1.0, 0.0,
		 1.0, 1.0
	};


	//make a 3x3 checkerboard texture to
	//really demonstrate GL_LINEAR vs GL_NEAREST
	//ABGR because LSB machine
	uint32_t test_texture[9];
	for (int i=0; i<9; ++i) {
		if (i % 2)
			test_texture[i] = 0xFF000000;
		else
			test_texture[i] = ~0x0;
	}



	glGenTextures(NUM_TEXTURES, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	if (!load_texture2D("../media/textures/test1.jpg", GL_NEAREST, GL_NEAREST, GL_MIRRORED_REPEAT, GL_FALSE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}


	glBindTexture(GL_TEXTURE_2D, textures[1]);

	if (!load_texture2D("../media/textures/test2.jpg", GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_FALSE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, test_texture);


	//start with tex0
	glBindTexture(GL_TEXTURE_2D, textures[0]);


	const char vs_file[] = "../media/shaders/texturing.vp";
	const char fs_file[] = "../media/shaders/texturing.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);
	int loc = glGetUniformLocation(program, "color_map");
	glUniform1i(loc, 0);


	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	GLuint square;
	glGenBuffers(1, &square);
	glBindBuffer(GL_ARRAY_BUFFER, square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_n_tex), points_n_tex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*12));


	mat4 mvp_mat(1);
	int mvp_loc = glGetUniformLocation(program, "mvp_mat");



	unsigned int old_time = 0, new_time=0, counter = 0, last_time=0;
	while (1) {
		new_time = SDL_GetTicks();
		if (handle_events(last_time, new_time, mvp_mat))
			break;

		last_time = new_time;

		if (new_time - old_time > 3000) {
			printf("%f FPS\n", counter*1000.f/(new_time-old_time));
			old_time = new_time;
			counter = 0;
		}

		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mvp_mat);

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		SDL_GL_SwapWindow(window);

		last_time = new_time;
		++counter;
	}

	glDeleteBuffers(1, &square);
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

	window = SDL_CreateWindow("Texturing", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

int handle_events(unsigned int last_time, unsigned int cur_time, mat4& mvp_mat)
{
	SDL_Event e;
	int sc;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_KEYDOWN:
			sc = e.key.keysym.scancode;

			switch (sc) {
			case SDL_SCANCODE_ESCAPE:
				return 1;
			case SDL_SCANCODE_1:
				tex_index = (tex_index + 1) % NUM_TEXTURES;
				glBindTexture(GL_TEXTURE_2D, textures[tex_index]);
				break;
			case SDL_SCANCODE_F:
			{
				int filter;
				if (tex_filter == 0) {
					filter = GL_LINEAR;
					puts("GL_LINEAR");
				} else {
					filter = GL_NEAREST;
					puts("GL_NEAREST");
				}
				for (int i=0; i<NUM_TEXTURES; ++i) {
					glBindTexture(GL_TEXTURE_2D, textures[i]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
				}
				tex_filter = !tex_filter;
				glBindTexture(GL_TEXTURE_2D, textures[tex_index]);
			}
			default:
				;
			}


			break; //sdl_keydown

		case SDL_QUIT:
			return 1;
			break;
		}
	}

	//SDL_PumpEvents() is called above in SDL_PollEvent()
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	float time = (cur_time - last_time)/1000.0f;

#define MOVE_SPEED DEG_TO_RAD(30)

	mat4 tmp(1);

	if (state[SDL_SCANCODE_LEFT]) {
		rsw::load_rotation_mat4(tmp, vec3(0, 0, 1), time * MOVE_SPEED);
	}
	if (state[SDL_SCANCODE_RIGHT]) {
		rsw::load_rotation_mat4(tmp, vec3(0, 0, 1), -time * MOVE_SPEED);
	}
	if (state[SDL_SCANCODE_UP]) {
		tmp = rsw::scale_mat4(1.01f, 1.01f, 1);
	}
	if (state[SDL_SCANCODE_DOWN]) {
		tmp = rsw::scale_mat4(0.99f, 0.99f, 1);
	}

	mvp_mat = mvp_mat * tmp;


	return 0;
}













