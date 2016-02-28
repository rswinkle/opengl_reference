#include <c_utils.h>

#include <glm_glframe.h>

#include <gltools.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>

//included in SDL.h?
//#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <stdio.h>

#include <iostream>
#include <vector>

#define WIDTH 640
#define HEIGHT 480

//TODO
#define RM_PI (3.14159265358979323846)
#define RM_2PI (2.0 * RM_PI)
#define PI_DIV_180 (0.017453292519943296)
#define INV_PI_DIV_180 (57.2957795130823229)

#define DEG_TO_RAD(x)  ((x)*PI_DIV_180)
#define RAD_TO_DEG(x)  ((x)*INV_PI_DIV_180)

#define MAX(a, b)  ((a) > (b)) ? (a) : (b)
#define MIN(a, b)  ((a) < (b)) ? (a) : (b)

using namespace std;

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events(GLFrame& camera_frame, unsigned int last_time, unsigned int cur_time);





int polygon_mode;


enum Control_Names {
	LEFT=0,
	RIGHT,
	FORWARD,
	BACK,
	UP,
	DOWN,
	TILTLEFT,
	TILTRIGHT,

	HIDECURSOR,
	FOVUP,
	FOVDOWN,
	ZMINUP,
	ZMINDOWN,
	PROVOKING,
	INTERPOLATION,
	SHADER,
	DEPTHTEST,
	POLYGONMODE,

	NCONTROLS
};


SDL_Scancode controls[NCONTROLS] =
{
	SDL_SCANCODE_A,
	SDL_SCANCODE_D,
	SDL_SCANCODE_W,
	SDL_SCANCODE_S,
	SDL_SCANCODE_LSHIFT,
	SDL_SCANCODE_SPACE,
	SDL_SCANCODE_Q,
	SDL_SCANCODE_E
};





int main(int argc, char** argv)
{
	setup_context();

	polygon_mode = 2;

	GLfloat grass_blade[] =
	{
		-0.3f,  0.0f,
		 0.3f,  0.0f,
		-0.20f, 1.0f,
		 0.1f,  1.3f,
		-0.05f, 2.3f,
		 0.0f,  3.3f
	};


	GLuint vao, buffer;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grass_blade), grass_blade, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint prog = load_shader_file_pair("../media/shaders/grass.vp", "../media/shaders/simple_color.fp");




	glUseProgram(prog);

	mat4 proj_mat = glm::perspective(glm::quarter_pi<float>(), WIDTH/(float)HEIGHT, 0.1f, 1000.0f);
	mat4 view_mat;

	mat4 mvp_mat;

	int mvp_loc = glGetUniformLocation(prog, "mvp_mat");

	int color_loc = glGetUniformLocation(prog, "color");

	vec4 Red(1, 0, 0, 1);
	vec4 Green(0, 1, 0, 1);
	vec4 Blue(0, 0, 1, 1);

	glUniform4fv(color_loc, 1, glm::value_ptr(Green));


	GLFrame camera(true);



	glEnable(GL_DEPTH_TEST);
	
	SDL_SetRelativeMouseMode(SDL_TRUE);

	unsigned int old_time = 0, new_time=0, counter = 0, last_time = SDL_GetTicks();
	while (1) {
		new_time = SDL_GetTicks();
		if (handle_events(camera, last_time, new_time))
			break;

		last_time = new_time;

		if (new_time - old_time > 3000) {
			printf("%f FPS\n", counter*1000.f/(new_time-old_time));
			old_time = new_time;
			counter = 0;
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


		//these are the same barring -0 vs 0 in a few positions which doesn't affect it
		//view_mat = glm::lookAt(camera.origin, camera.origin+camera.forward, camera.up);
		view_mat = camera.get_camera_matrix();

		mvp_mat = proj_mat * view_mat;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp_mat));

		
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, 256*256);

		SDL_GL_SwapWindow(window);

		last_time = new_time;
		++counter;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(prog);

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

	window = SDL_CreateWindow("Flying", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

int handle_events(GLFrame& camera_frame, unsigned int last_time, unsigned int cur_time)
{
	SDL_Event event;
	int sc;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			sc = event.key.keysym.scancode;

			if (sc == SDL_SCANCODE_ESCAPE) {
				return 1;
			} else if (sc == SDL_SCANCODE_P) {
				polygon_mode = (polygon_mode + 1) % 3;
				if (polygon_mode == 0)
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				else if (polygon_mode == 1)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			break;

		case SDL_MOUSEMOTION:
		{
			//printf("%d %d %d %d\n", event.motion.y, event.motion.x, event.motion.xrel, event.motion.yrel);
			float dx = event.motion.xrel;
			float dy = event.motion.yrel;
			
			camera_frame.rotate_local_y(DEG_TO_RAD(-dx/50));
			camera_frame.rotate_local_x(DEG_TO_RAD(dy/25));
			
			if (9 < dx*dx + dy*dy) {
				camera_frame.rotate_local_y(DEG_TO_RAD(-dx/30));
				camera_frame.rotate_local_x(DEG_TO_RAD(dy/25));
				//mousex = width/2;
				//mousey = height/2;
			}
		}
			break;

		case SDL_QUIT:
			return 1;
		}
	}



	//SDL_PumpEvents() is called above in SDL_PollEvent()
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	float time = (cur_time - last_time)/1000.0f;

#define MOVE_SPEED 20
	
	if (state[controls[LEFT]]) {
		camera_frame.move_right(time * MOVE_SPEED);
	}
	if (state[controls[RIGHT]]) {
		camera_frame.move_right(time * -MOVE_SPEED);
	}
	if (state[controls[UP]]) {
		camera_frame.move_up(time * MOVE_SPEED);
	}
	if (state[controls[DOWN]]) {
		camera_frame.move_up(time * -MOVE_SPEED);
	}
	if (state[controls[FORWARD]]) {
		camera_frame.move_forward(time*20);
	}
	if (state[controls[BACK]]) {
		camera_frame.move_forward(time*-20);
	}
	if (state[controls[TILTLEFT]]) {
		camera_frame.rotate_local_z(DEG_TO_RAD(-60*time));
	}
	if (state[controls[TILTRIGHT]]) {
		camera_frame.rotate_local_z(DEG_TO_RAD(60*time));
	}


	return 0;
}




