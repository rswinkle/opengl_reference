#include <gltools.h>
#include <glm_matstack.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <iostream>
#include <stdio.h>
#include <vector>

#define WIDTH 640
#define HEIGHT 480

using namespace std;

using glm::ivec3;
using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::mat4;


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events(unsigned int elapsed);
void setup_buffers();
void draw_scene_on_screen();

float z;
int polygon_mode;

GLuint cube_vao, moon_vao, macbook_vao;
GLuint macbook_screen_vao;
int cube_tris, moon_tris, macbook_tris;

int fb_w = 512, fb_h = 512;

// uniforms
mat4 uMVMatrix;
mat4 uPMatrix;
mat3 uNMatrix;
vec3 uAmbientColor(0.2);
vec3 uPointLight(0.0f, 0.0f, -5.0f);
vec3 uPointLightColor(0.8);
int uUseTextures = 1; //bool
int uSampler;


float laptop_scr_aspect_ratio = 1.66;
float moonAngle = 180;
float cubeAngle = 0;

matrix_stack mvstack;
GLuint program;
GLuint box_tex, moon_tex;

int main(int argc, char** argv)
{
	setup_context();

	setup_buffers();

	polygon_mode = 2;


	glGenTextures(1, &box_tex);
	glBindTexture(GL_TEXTURE_2D, box_tex);
	if (!load_texture2D("../media/textures/crate.gif", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_REPEAT, GL_TRUE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	glGenTextures(1, &moon_tex);
	glBindTexture(GL_TEXTURE_2D, moon_tex);
	if (!load_texture2D("../media/textures/moon.gif", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_REPEAT, GL_TRUE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	GLuint framebuffer, framebuffer_tex;
	//glCreateFramebuffers(1, &framebuffer);
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &framebuffer_tex);
	glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb_w, fb_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_tex, 0);

	// rbo for depth/-stencil
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb_w, fb_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// TODO try texture for depth/stencil sometime
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, fb_w, fb_h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is not complete!\n");
		exit(0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	// phong shaders
	const char frag_vs_file[] = "../media/shaders/lesson16.vp";
	const char frag_fs_file[] = "../media/shaders/lesson16.fp";

	program = load_shader_file_pair(frag_vs_file, frag_fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	uPMatrix = glm::perspective(glm::radians(45.0f), WIDTH/(float)HEIGHT, 0.1f, 100.0f);

	// These never change
	glUseProgram(program);
	set_uniform1i(program, "uSampler", 0);
	set_uniform_mat4f(program, "uPMatrix", glm::value_ptr(uPMatrix));

	glClearColor(0,0,0,1);

	float laptopAngle = 0;

	glEnable(GL_DEPTH_TEST);

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

		moonAngle += 0.05 * elapsed;
		cubeAngle += 0.05 * elapsed;
		laptopAngle -= 0.005 * elapsed;

		glUseProgram(program);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		draw_scene_on_screen();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		mvstack.load_identity();
		mvstack.push();

		mvstack.translate(0, -0.4, -1.7);
		mvstack.rotate(glm::radians(laptopAngle), vec3(0, 1, 0));
		mvstack.rotate(glm::radians(-90.0f), vec3(1,0,0));

		set_uniform_mat4f(program, "uMVMatrix", glm::value_ptr(mvstack.get_matrix()));
		uNMatrix = glm::transpose(glm::inverse(mat3(mvstack.get_matrix())));
		set_uniform_mat3f(program, "uNMatrix", glm::value_ptr(uNMatrix));

		set_uniform1i(program, "uShowSpec", true);
		set_uniform3f(program, "uPntLight", -1, 2, -1);
		set_uniform3f(program, "uAmbLight", 0.2, 0.2, 0.2);
		set_uniform3f(program, "uPntLightDiff", 0.8, 0.8, 0.8);
		set_uniform3f(program, "uPntLightSpec", 0.8, 0.8, 0.8);

		// The laptop body is quite shiny and has no texture.  It reflects lots of specular light
		set_uniform3f(program, "uMatAmbient", 1, 1, 1);
		set_uniform3f(program, "uMatDiff", 1, 1, 1);
		set_uniform3f(program, "uMatSpec", 1.5, 1.5, 1.5);
		set_uniform1f(program, "uMatShininess", 5);
		set_uniform3f(program, "uMatEmissive", 0, 0, 0);
		set_uniform1i(program, "uUseTextures", false);

		glBindVertexArray(macbook_vao);
		glDrawElements(GL_TRIANGLES, macbook_tris*3, GL_UNSIGNED_INT, 0);

		set_uniform3f(program, "uMatAmbient", 0, 0, 0);
		set_uniform3f(program, "uMatDiff", 0, 0, 0);
		set_uniform3f(program, "uMatSpec", 0.5, 0.5, 0.5);
		set_uniform1f(program, "uMatShininess", 20);
		set_uniform3f(program, "uMatEmissive", 1.5, 1.5, 1.5);
		set_uniform1i(program, "uUseTextures", true);

		glBindVertexArray(macbook_screen_vao);
		glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		mvstack.pop();

		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &moon_vao);
	glDeleteVertexArrays(1, &cube_vao);
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

	window = SDL_CreateWindow("Lesson 16", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

			} else if (sc == SDL_SCANCODE_LEFT) {
			} else if (sc == SDL_SCANCODE_RIGHT) {
			} else if (sc == SDL_SCANCODE_UP) {
			} else if (sc == SDL_SCANCODE_DOWN) {
			} else if (sc == SDL_SCANCODE_P) {
				polygon_mode = (polygon_mode + 1) % 3;
				if (polygon_mode == 0)
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				else if (polygon_mode == 1)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		} else if (e.type == SDL_MOUSEMOTION) {
		}
	}

	//SDL_PumpEvents() is called above in SDL_PollEvent()
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_EQUALS]) {
		z += 0.05;
	} else if (state[SDL_SCANCODE_MINUS]) {
		z -= 0.05;
	}

	return 0;
}


void setup_buffers()
{
	float cube_vertices[] = {
		// Front face
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// Back face
		-1.0, -1.0, -1.0,
		-1.0,  1.0, -1.0,
		 1.0,  1.0, -1.0,
		 1.0, -1.0, -1.0,
		// Top face
		-1.0,  1.0, -1.0,
		-1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0, -1.0,
		// Bottom face
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
		// Right face
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		 1.0,  1.0,  1.0,
		 1.0, -1.0,  1.0,
		// Left face
		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0
	};

	float cube_normals[] = {
		// Front face
		 0.0,  0.0,  1.0,
		 0.0,  0.0,  1.0,
		 0.0,  0.0,  1.0,
		 0.0,  0.0,  1.0,
		// Back face
		 0.0,  0.0, -1.0,
		 0.0,  0.0, -1.0,
		 0.0,  0.0, -1.0,
		 0.0,  0.0, -1.0,
		// Top face
		 0.0,  1.0,  0.0,
		 0.0,  1.0,  0.0,
		 0.0,  1.0,  0.0,
		 0.0,  1.0,  0.0,
		// Bottom face
		 0.0, -1.0,  0.0,
		 0.0, -1.0,  0.0,
		 0.0, -1.0,  0.0,
		 0.0, -1.0,  0.0,
		// Right face
		 1.0,  0.0,  0.0,
		 1.0,  0.0,  0.0,
		 1.0,  0.0,  0.0,
		 1.0,  0.0,  0.0,
		// Left face
		-1.0,  0.0,  0.0,
		-1.0,  0.0,  0.0,
		-1.0,  0.0,  0.0,
		-1.0,  0.0,  0.0
	};

	float cube_texcoords[] = {
		// Front face
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// Back face
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		// Top face
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		// Bottom face
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		// Right face
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		// Left face
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0
	};

	GLuint cube_triangles[] = {
		0, 1, 2,      0, 2, 3,    // Front face
		4, 5, 6,      4, 6, 7,    // Back face
		8, 9, 10,     8, 10, 11,  // Top face
		12, 13, 14,   12, 14, 15, // Bottom face
		16, 17, 18,   16, 18, 19, // Right face
		20, 21, 22,   20, 22, 23  // Left face
	};

	cube_tris = 12;

	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	GLuint vert_buf;
	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint normal_buf;
	glGenBuffers(1, &normal_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tex_buf;
	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint elem_buf;
	glGenBuffers(1, &elem_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangles), cube_triangles, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glGenVertexArrays(1, &moon_vao);
	glBindVertexArray(moon_vao);

	vector<vec3> verts;
	vector<vec3> normals;
	vector<vec2> texcoords;
	vector<ivec3> tris;

	float lat_bands = 30;
	float long_bands = 30;
	float radius = 1;

	for (float lat_n = 0; lat_n <= lat_bands; lat_n++) {
		float theta = lat_n * glm::pi<float>() / lat_bands;
		float sin_theta = sin(theta);
		float cos_theta = cos(theta);
		
		for (float long_n=0; long_n <= long_bands; long_n++) {
			float phi = long_n * 2 * glm::pi<float>() / long_bands;
			float sin_phi = sin(phi);
			float cos_phi = cos(phi);

			float x = cos_phi * sin_theta;
			float y = cos_theta;
			float z = sin_phi * sin_theta;
			float u = 1 - (long_n / long_bands);
			float v = 1 - (lat_n / lat_bands);

			normals.push_back(vec3(x, y, z));
			texcoords.push_back(vec2(u, v));
			verts.push_back(vec3(radius*x, radius*y, radius*z));
		}
	}

	for (int i=0; i<lat_bands; i++) {
		for (int j=0; j<long_bands; j++) {
			float first = (i * (long_bands+1)) + j;
			float second = first + long_bands + 1;

			// original CW winding, means CULL_FACE messes it up
			//tris.push_back(ivec3(first, second, first+1));
			//tris.push_back(ivec3(second, second+1, first+1));
			
			tris.push_back(ivec3(first, first+1, second));
			tris.push_back(ivec3(second, first+1, second+1));
		}
	}
	moon_tris = tris.size();

	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(vec3), &verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &normal_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(vec2), &texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &elem_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tris.size()*sizeof(ivec3), &tris[0], GL_STATIC_DRAW);

	//TODO necessary?
	glBindVertexArray(0);

#include "../media/models/macbook.h"

	glGenVertexArrays(1, &macbook_vao);
	glBindVertexArray(macbook_vao);

	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &normal_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals), vertexNormals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTextureCoords), vertexTextureCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &elem_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	macbook_tris = sizeof(indices)/sizeof(indices[0])/3;

	glBindVertexArray(0);

	glGenVertexArrays(1, &macbook_screen_vao);
	glBindVertexArray(macbook_screen_vao);

	float screen_vertices[] =
	{
		 0.580687, 0.659, 0.813106,
		-0.580687, 0.659, 0.813107,
		 0.580687, 0.472, 0.113121,
		-0.580687, 0.472, 0.113121
	};
	float screen_vertexNormals[] =
	{
		 0.000000, -0.965926, 0.258819,
		 0.000000, -0.965926, 0.258819,
		 0.000000, -0.965926, 0.258819,
		 0.000000, -0.965926, 0.258819
	};
	float screen_textureCoords[] =
	{
		1.0, 1.0,
		0.0, 1.0,
		1.0, 0.0,
		0.0, 0.0
	};
	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &normal_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertexNormals), screen_vertexNormals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_textureCoords), screen_textureCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

}

void draw_scene_on_screen()
{
	glViewport(0, 0, fb_w, fb_h);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	set_uniform3f(program, "uAmbLight", 0.2, 0.2, 0.2);

	set_uniform3f(program, "uPntLight", 0, 0, -5);
	set_uniform3f(program, "uPntLightDiff", 0.8, 0.8, 0.8);
	set_uniform1i(program, "uShowSpec", false);
	set_uniform1i(program, "uUseTextures", true);

	set_uniform3f(program, "uMatAmbient", 1, 1, 1);
	set_uniform3f(program, "uMatDiff", 1, 1, 1);
	set_uniform3f(program, "uMatSpec", 0, 0, 0);
	set_uniform1f(program, "uMatShininess", 0);
	set_uniform3f(program, "uMatEmissive", 0, 0, 0);

	mvstack.load_identity();
	mvstack.translate(vec3(0,0,-5));
	mvstack.rotate(glm::radians(30.0f), vec3(1, 0, 0));

	mvstack.push();
	mvstack.rotate(glm::radians(moonAngle), vec3(0, 1, 0));
	mvstack.translate(2, 0, 0);

	glBindTexture(GL_TEXTURE_2D, moon_tex);

	set_uniform_mat4f(program, "uMVMatrix", glm::value_ptr(mvstack.get_matrix()));
	uNMatrix = glm::transpose(glm::inverse(mat3(mvstack.get_matrix())));
	set_uniform_mat3f(program, "uNMatrix", glm::value_ptr(uNMatrix));

	glBindVertexArray(moon_vao);
	glDrawElements(GL_TRIANGLES, moon_tris*3, GL_UNSIGNED_INT, 0);

	mvstack.pop();

	mvstack.push();
	glBindTexture(GL_TEXTURE_2D, box_tex);

	mvstack.rotate(glm::radians(cubeAngle), vec3(0, 1, 0));
	mvstack.translate(1.25, 0, 0);

	// Only MVmatrix and NMatrix change
	set_uniform_mat4f(program, "uMVMatrix", glm::value_ptr(mvstack.get_matrix()));
	uNMatrix = glm::transpose(glm::inverse(mat3(mvstack.get_matrix())));
	set_uniform_mat3f(program, "uNMatrix", glm::value_ptr(uNMatrix));

	glBindVertexArray(cube_vao);
	glDrawElements(GL_TRIANGLES, cube_tris*3, GL_UNSIGNED_INT, 0);

	mvstack.pop();
}





