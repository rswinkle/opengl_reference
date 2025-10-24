#include <gltools.h>
#include <glm_matstack.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <iostream>
#include <stdio.h>
#include <vector>


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gl3.h"

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024


#define WIDTH 800
#define HEIGHT 480

#define GUI_W 200

using namespace std;

using glm::ivec3;
using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::mat4;


SDL_Window* window;
SDL_GLContext glcontext;
struct nk_context* ctx;

void cleanup();
void setup_context();
int handle_events(unsigned int elapsed);
void setup_buffers();

float z;
int polygon_mode;

GLuint cube_vao, moon_vao;
int cube_tris, moon_tris;

// uniforms
mat4 uMVMatrix;
mat4 uPMatrix;
mat3 uNMatrix;
vec3 uAmbientColor(0.2);
vec3 uPointLight(0.0f, 0.0f, -20.0f);
vec3 uPointLightColor(0.8);
int uUseLighting = 1; //bool
int uUseTextures = 1; //bool
int uSampler;

int uMVMatrix_loc;
int uPMatrix_loc;
int uNMatrix_loc;
int uAmbientColor_loc;
int uPointLight_loc;
int uPointLightColor_loc;
int uUseLighting_loc;
int uUseTextures_loc;
int uSampler_loc;

int main(int argc, char** argv)
{
	setup_context();

	setup_buffers();

	polygon_mode = 2;


	GLuint box_tex, moon_tex;
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

	// gouraud shaders
	const char vert_vs_file[] = "../media/shaders/lesson13_per_vertex_lighting.vp";
	const char vert_fs_file[] = "../media/shaders/lesson13_per_vertex_lighting.fp";

	GLuint vertex_lighting = load_shader_file_pair(vert_vs_file, vert_fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	// phong shaders
	const char frag_vs_file[] = "../media/shaders/lesson13_per_vertex_lighting.vp";
	const char frag_fs_file[] = "../media/shaders/lesson13_per_vertex_lighting.fp";

	GLuint frag_lighting = load_shader_file_pair(frag_vs_file, frag_fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);

	uMVMatrix_loc = glGetUniformLocation(program, "uMVMatrix");
	uPMatrix_loc = glGetUniformLocation(program, "uPMatrix");
	uNMatrix_loc = glGetUniformLocation(program, "uNMatrix");
	uAmbientColor_loc = glGetUniformLocation(program, "uAmbientColor");
	uPointLight_loc = glGetUniformLocation(program, "uPointLightingLocation");
	uPointLightColor_loc = glGetUniformLocation(program, "uPointLightingColor");
	uUseLighting_loc = glGetUniformLocation(program, "uUseLighting");
	uUseTextures_loc = glGetUniformLocation(program, "uUseTextures");
	uSampler_loc = glGetUniformLocation(program, "uSampler");

	check_errors(0, "uniform locs");

	glUniform1i(uSampler_loc, 0);
	glClearColor(0,0,0,1);

	float moonAngle = 180;
	float cubeAngle = 0;

	matrix_stack mvstack;
	mvstack.load_identity();
	mvstack.translate(vec3(0,0,-20));


	//SDL_SetRelativeMouseMode(SDL_TRUE);

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

		// reset state every frame due to GUI, TODO double check exactly what changes
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glUseProgram(program);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, moon_tex);

		uPMatrix = glm::perspective(glm::radians(45.0f), WIDTH/(float)HEIGHT, 0.1f, 100.0f);

		mvstack.push();
		mvstack.rotate(glm::radians(moonAngle), vec3(0, 1, 0));
		mvstack.translate(5, 0, 0);

		glUniformMatrix4fv(uMVMatrix_loc, 1, GL_FALSE, glm::value_ptr(mvstack.get_matrix()));
		glUniformMatrix4fv(uPMatrix_loc, 1, GL_FALSE, glm::value_ptr(uPMatrix));

		// transpose/inverse not necessary here but meh
		//uNMatrix = mat3(uMVMatrix);
		uNMatrix = glm::transpose(glm::inverse(mat3(mvstack.get_matrix())));
		glUniformMatrix3fv(uNMatrix_loc, 1, GL_FALSE, glm::value_ptr(uNMatrix));

		glUniform1i(uSampler_loc, 0); // never changes
		glUniform3fv(uAmbientColor_loc, 1, glm::value_ptr(uAmbientColor));

		glUniform3fv(uPointLight_loc, 1, glm::value_ptr(uPointLight));
		glUniform3fv(uPointLightColor_loc, 1, glm::value_ptr(uPointLightColor));
		glUniform1i(uUseLighting_loc, uUseLighting);

		glBindVertexArray(moon_vao);
		glDrawElements(GL_TRIANGLES, moon_tris*3, GL_UNSIGNED_INT, 0);

		mvstack.pop();

		mvstack.push();
		glBindTexture(GL_TEXTURE_2D, box_tex);

		mvstack.rotate(glm::radians(cubeAngle), vec3(0, 1, 0));
		mvstack.translate(5, 0, 0);

		// Only MVmatrix and NMatrix change
		glUniformMatrix4fv(uMVMatrix_loc, 1, GL_FALSE, glm::value_ptr(mvstack.get_matrix()));
		//glUniformMatrix4fv(uPMatrix_loc, 1, GL_FALSE, glm::value_ptr(uPMatrix));

		// transpose/inverse not necessary here but meh
		//uNMatrix = mat3(uMVMatrix);
		uNMatrix = glm::transpose(glm::inverse(mat3(mvstack.get_matrix())));
		glUniformMatrix3fv(uNMatrix_loc, 1, GL_FALSE, glm::value_ptr(uNMatrix));

		glBindVertexArray(cube_vao);
		glDrawElements(GL_TRIANGLES, cube_tris*3, GL_UNSIGNED_INT, 0);

		mvstack.pop();

		if (nk_begin(ctx, "Controls", nk_rect(WIDTH-GUI_W, 0, GUI_W, HEIGHT),
		    NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
		{
			static struct nk_colorf pt_lt_color = { 0.8, 0.8, 0.8, 1 };
			static struct nk_colorf ambient_color = { 0.2, 0.2, 0.2, 1 };
			//nk_layout_row_static(ctx, 30, GUI_W, 1);
			nk_layout_row_dynamic(ctx, 0, 1);
			if (nk_checkbox_label(ctx, "Use Lighting", &uUseLighting)) {
				printf("Lighting %s\n", (uUseLighting ? "On" : "Off"));
			}

			nk_label(ctx, "Point Light", NK_TEXT_CENTERED);
			nk_label(ctx, "Location", NK_TEXT_CENTERED);

			//nk_layout_row_dynamic(ctx, 0, 2);

			nk_layout_row_template_begin(ctx, 0);
			nk_layout_row_template_push_static(ctx, 20);
			nk_layout_row_template_push_dynamic(ctx);
			nk_layout_row_template_end(ctx);

			nk_label(ctx, "X", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -40.0, &uPointLight.x, 20.0, 1.0, 0.2);
			nk_label(ctx, "Y", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -40.0, &uPointLight.y, 20.0, 1.0, 0.2);
			nk_label(ctx, "Z", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -40.0, &uPointLight.z, 20.0, 1.0, 0.2);


			//nk_layout_row_static(ctx, 30, GUI_W, 1);
			nk_layout_row_dynamic(ctx, 0, 1);

			nk_label(ctx, "Color", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 100, 1);
			pt_lt_color = nk_color_picker(ctx, pt_lt_color, NK_RGB);
			nk_layout_row_dynamic(ctx, 0, 1);
			pt_lt_color.r = nk_propertyf(ctx, "#R:", 0, pt_lt_color.r, 1.0f, 0.01f,0.005f);
			pt_lt_color.g = nk_propertyf(ctx, "#G:", 0, pt_lt_color.g, 1.0f, 0.01f,0.005f);
			pt_lt_color.b = nk_propertyf(ctx, "#B:", 0, pt_lt_color.b, 1.0f, 0.01f,0.005f);

			uPointLightColor = vec3(pt_lt_color.r, pt_lt_color.g, pt_lt_color.b);


			nk_label(ctx, "Ambient Light", NK_TEXT_CENTERED);
			nk_label(ctx, "Color", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 100, 1);
			ambient_color = nk_color_picker(ctx, ambient_color, NK_RGB);
			nk_layout_row_dynamic(ctx, 0, 1);
			ambient_color.r = nk_propertyf(ctx, "#R:", 0, ambient_color.r, 1.0f, 0.01f,0.005f);
			ambient_color.g = nk_propertyf(ctx, "#G:", 0, ambient_color.g, 1.0f, 0.01f,0.005f);
			ambient_color.b = nk_propertyf(ctx, "#B:", 0, ambient_color.b, 1.0f, 0.01f,0.005f);

			uAmbientColor = vec3(ambient_color.r, ambient_color.g, ambient_color.b);

			if (nk_button_label(ctx, "button"))
				printf("button pressed!\n");
		}
		nk_end(ctx);

		/* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
		 * with blending, scissor, face culling, depth test and viewport and
		 * defaults everything back into a default state.
		 * Make sure to either a.) save and restore or b.) reset your own state after
		 * rendering the UI. */
		nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

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

	window = SDL_CreateWindow("Lesson 13", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

    ctx = nk_sdl_init(window);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_sdl_font_stash_end();
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &roboto->handle);*/}

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
	nk_input_begin(ctx);
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
		nk_sdl_handle_event(&e);
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
	float vertices[] = {
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

	GLuint triangles[] = {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glGenVertexArrays(1, &moon_vao);
	glBindVertexArray(moon_vao);

	vector<vec3> verts;
	vector<vec3> normals;
	vector<vec2> texcoords;
	vector<ivec3> tris;

	float lat_bands = 30;
	float long_bands = 30;
	float radius = 2;

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

	glBindVertexArray(0);
}





