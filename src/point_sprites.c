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



int main(int argc, char** argv)
{
	setup_context();

	// Apparently, even if the point itself (ie the center of a point sprite) would
	// have been clipped, it is still rendered as if it were a quad with part
	// of it off screen
	float points[] = { -0.5, -0.5, 0,
	                    0.5, -0.5, 0,
	                    0,    1.2, 0 };


	GLuint textures[2];
	glGenTextures(2, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	if (!load_texture2D("../media/textures/test1.jpg", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	if (!load_texture2D("../media/textures/clouds.tga", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	GLuint simple_prog = load_shader_file_pair("../media/shaders/passthrough.vp", "../media/shaders/point_sprites.fp");
	if (!simple_prog) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	GLuint texture_prog = load_shader_file_pair("../media/shaders/passthrough.vp", "../media/shaders/tex_point_sprites.fp");
	if (!texture_prog) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(texture_prog);
	set_uniform1i(texture_prog, "tex", 0);
	set_uniform1i(texture_prog, "dissolve_tex", 1);
	

	glUseProgram(simple_prog);

	//no default vao in core profile ...
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint point_buf;
	glGenBuffers(1, &point_buf);
	glBindBuffer(GL_ARRAY_BUFFER, point_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	float ret_dat[2];
	glGetFloatv(GL_POINT_SIZE, ret_dat);
	printf("point size = %f\n", ret_dat[0]);

	glGetFloatv(GL_POINT_SIZE_RANGE, ret_dat);
	printf("point size range  = [%f - %f]\n", ret_dat[0], ret_dat[1]);

	glGetFloatv(GL_POINT_SIZE_GRANULARITY, ret_dat);
	printf("point size granularity = %f\n", ret_dat[0]);

	glPointSize(120.0);

	glGetFloatv(GL_POINT_SIZE, ret_dat);
	printf("point size = %f\n", ret_dat[0]);

	check_errors(0, NULL);

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

		float diss_factor = fmodf(new_time/1000.0f, 10.0f);
		diss_factor /= 10.0f;

		set_uniform1f(texture_prog, "dissolve_factor", diss_factor);
		
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(simple_prog);
		glDrawArrays(GL_POINTS, 0, 2);

		glUseProgram(texture_prog);
		glDrawArrays(GL_POINTS, 2, 1);

		SDL_GL_SwapWindow(window);
	}

	glDeleteBuffers(1, &point_buf);
	glDeleteProgram(simple_prog);

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

	window = SDL_CreateWindow("Point Sprites", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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




