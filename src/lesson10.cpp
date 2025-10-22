#include <gltools.h>
#include <c_utils.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <vector>

#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

using namespace std;

using glm::vec2;
using glm::vec3;
using glm::mat4;


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events(unsigned int elapsed);
int load_world(const char* path, vector<vec3>& verts, vector<vec2>& texcoords);

float pitch, pitchRate;
float yaw, yawRate;

float xPos = 0;
float yPos = 0.4;
float zPos = -0.5;

float speed = 0;


// uniforms
mat4 uMVMatrix;
mat4 uPMatrix;
int uSampler;

int uMVMatrix_loc;
int uPMatrix_loc;
int uSampler_loc;


int main(int argc, char** argv)
{
	setup_context();

	vector<vec3> verts;
	vector<vec2> texcoords;

	load_world("../media/models/world.txt", verts, texcoords);


	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (!load_texture2D("../media/textures/mud.gif", GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_TRUE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vert_buf;
	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(vec3), &verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tex_buf;
	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(vec2), &texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//GLuint elem_buf;
	//glGenBuffers(1, &elem_buf);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	glBindVertexArray(0);

	const char vs_file[] = "../media/shaders/lesson10.vp";
	const char fs_file[] = "../media/shaders/lesson10.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(program);

	mat4 mvp_mat(1);
	uMVMatrix_loc = glGetUniformLocation(program, "uMVMatrix");
	uPMatrix_loc = glGetUniformLocation(program, "uPMatrix");
	uSampler_loc = glGetUniformLocation(program, "uSampler");

	check_errors(0, "uniform locs");

	glUniform1i(uSampler_loc, 0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);

	float joggingAngle = 0;
	unsigned int old_time = 0, new_time=0, counter = 0, elapsed;
	while (1) {
		new_time = SDL_GetTicks();
		elapsed = new_time - old_time;
		if (handle_events(elapsed))
			break;

		old_time = new_time;

		counter++;
		if (elapsed > 3000) {
			printf("%f FPS\n", counter*1000.f/(elapsed));
			old_time = new_time;
			counter = 0;
		}

		if (speed) {
			xPos -= sin(glm::radians(yaw)) * speed * elapsed;
			zPos -= cos(glm::radians(yaw)) * speed * elapsed;

			joggingAngle += elapsed * 0.6; // 0.6 "fiddle factor" - makes it feel more realistic :-)
			yPos = sin(glm::radians(joggingAngle)) / 20 + 0.4;
		}
		yaw += yawRate * elapsed;
		pitch += pitchRate * elapsed;


		uPMatrix = glm::perspective(glm::radians(45.0f), WIDTH/(float)HEIGHT, 0.1f, 100.0f);
		uMVMatrix = glm::rotate(mat4(1), glm::radians(-pitch), vec3(1, 0, 0));
		uMVMatrix = glm::rotate(uMVMatrix, glm::radians(-yaw), vec3(0, 1, 0));
		uMVMatrix = glm::translate(uMVMatrix, vec3(-xPos, -yPos, -zPos));

		glUniformMatrix4fv(uMVMatrix_loc, 1, GL_FALSE, glm::value_ptr(uMVMatrix));
		glUniformMatrix4fv(uPMatrix_loc, 1, GL_FALSE, glm::value_ptr(uPMatrix));

		glUniform1i(uSampler_loc, 0); // never changes


		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, verts.size());

		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vert_buf);
	glDeleteBuffers(1, &tex_buf);
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

	window = SDL_CreateWindow("Lesson 10", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

int handle_events(unsigned int elapsed)
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

			// since we're not doing acceleration we could use
			// key events instead of key state
			} else if (sc == SDL_SCANCODE_LEFT) {
			} else if (sc == SDL_SCANCODE_RIGHT) {
			} else if (sc == SDL_SCANCODE_UP) {
			} else if (sc == SDL_SCANCODE_DOWN) {
			}
		}
	}

	//SDL_PumpEvents() is called above in SDL_PollEvent()
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_PAGEUP]) {
		pitchRate = 0.1;
	} else if (state[SDL_SCANCODE_PAGEDOWN]) {
		pitchRate = -0.1;
	} else {
		pitchRate = 0;
	}

	if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {
		speed = 0.003;
	} else if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {
		speed = -0.003;
	} else {
		speed = 0;
	}

	if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
		yawRate = 0.1;
	} else if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
		yawRate = -0.1;
	} else {
		yawRate = 0;
	}

	return 0;
}


int load_world(const char* path, vector<vec3>& verts, vector<vec2>& texcoords)
{
	c_array lines, file_contents;
	
	if (!file_open_readlines(path, &lines, &file_contents)) {
		return 0;
	}
	// file is already closed

	vec3 v;
	vec2 t;

	char** lns = (char**)lines.data;
	for (int i=0; i<lines.len; i++) {
		// Honestly should just simplify the format to just geometry data
		// but for now we'll do this
		if (lns[i][0] && lns[i][0] != '/' && lns[i][0] != 'N') {
			if (5 != sscanf(lns[i], "%f %f %f %f %f", &v.x, &v.y, &v.z, &t.x, &t.y)) {
				printf("Error parsing %s line: %s\n", path, lns[i]);
				exit(1);
			}
			verts.push_back(v);
			texcoords.push_back(t);
		}
	}

	free(file_contents.data);
	free(lines.data);
	return 1;
}




