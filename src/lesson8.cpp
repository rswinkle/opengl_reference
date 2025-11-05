#include <gltools.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <iostream>
#include <stdio.h>

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

using glm::vec3;
using glm::mat3;
using glm::mat4;


SDL_Window* window;
SDL_GLContext glcontext;
struct nk_context* ctx;

void cleanup();
void setup_context();
int handle_events(unsigned int elapsed);

float z;
float x_rot, y_rot;
float x_speed = 3;
float y_speed = -3;

int use_blending = 1; // bool

// uniforms
mat4 uMVMatrix;
mat4 uPMatrix;
mat3 uNMatrix;
vec3 uAmbientColor(0.2);
vec3 uLightingDirection(-0.25, -0.25, -1.0);
vec3 uDirectionalColor(0.8);
int uUseLighting = 1; // bool
int uSampler;
float uAlpha = 0.5f;

int uMVMatrix_loc;
int uPMatrix_loc;
int uNMatrix_loc;
int uAmbientColor_loc;
int uLightingDirection_loc;
int uDirectionalColor_loc;
int uUseLighting_loc;
int uSampler_loc;
int uAlpha_loc;


int main(int argc, char** argv)
{
	setup_context();


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

	float normals[] = {
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

	float texcoords[] = {
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

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (!load_texture2D("../media/textures/glass.gif", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_REPEAT, GL_TRUE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vert_buf;
	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint normal_buf;
	glGenBuffers(1, &normal_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tex_buf;
	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint elem_buf;
	glGenBuffers(1, &elem_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	glBindVertexArray(0);

	const char vs_file[] = "../media/shaders/lesson7.vp";
	const char fs_file[] = "../media/shaders/lesson8.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(program);

	z = -5;

	mat4 mvp_mat(1);
	uMVMatrix_loc = glGetUniformLocation(program, "uMVMatrix");
	uPMatrix_loc = glGetUniformLocation(program, "uPMatrix");
	uNMatrix_loc = glGetUniformLocation(program, "uNMatrix");
	uAmbientColor_loc = glGetUniformLocation(program, "uAmbientColor");
	uLightingDirection_loc = glGetUniformLocation(program, "uLightingDirection");
	uDirectionalColor_loc = glGetUniformLocation(program, "uDirectionalColor");
	uUseLighting_loc = glGetUniformLocation(program, "uUseLighting");
	uSampler_loc = glGetUniformLocation(program, "uSampler");
	uAlpha_loc = glGetUniformLocation(program, "uAlpha");

	check_errors(0, "uniform locs");

	glUniform1i(uSampler_loc, 0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);

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

		// reset state every frame due to GUI
		//glEnable(GL_CULL_FACE);
		glUseProgram(program);
		glBindTexture(GL_TEXTURE_2D, texture);

		if (use_blending) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glUniform1f(uAlpha_loc, uAlpha);
		} else {
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}

		uPMatrix = glm::perspective(glm::radians(45.0f), WIDTH/(float)HEIGHT, 0.1f, 100.0f);
		uMVMatrix = glm::translate(mat4(1), vec3(0,0,z));
		uMVMatrix = glm::rotate(uMVMatrix, glm::radians(x_rot), vec3(1, 0, 0));
		uMVMatrix = glm::rotate(uMVMatrix, glm::radians(y_rot), vec3(0, 1, 0));

		glUniformMatrix4fv(uMVMatrix_loc, 1, GL_FALSE, glm::value_ptr(uMVMatrix));
		glUniformMatrix4fv(uPMatrix_loc, 1, GL_FALSE, glm::value_ptr(uPMatrix));

		// transpose/inverse not necessary here but meh
		//uNMatrix = mat3(uMVMatrix);
		uNMatrix = glm::transpose(glm::inverse(mat3(uMVMatrix)));
		glUniformMatrix3fv(uNMatrix_loc, 1, GL_FALSE, glm::value_ptr(uNMatrix));

		glUniform1i(uSampler_loc, 0); // never changes
		glUniform3fv(uAmbientColor_loc, 1, glm::value_ptr(uAmbientColor));

		vec3 adjusted_ld = glm::normalize(-uLightingDirection);
		glUniform3fv(uLightingDirection_loc, 1, glm::value_ptr(adjusted_ld));

		glUniform3fv(uDirectionalColor_loc, 1, glm::value_ptr(uDirectionalColor));
		glUniform1i(uUseLighting_loc, uUseLighting);


		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof(triangles), GL_UNSIGNED_INT, 0);


		if (nk_begin(ctx, "Controls", nk_rect(WIDTH-GUI_W, 0, GUI_W, HEIGHT),
		    NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
		{
			static struct nk_colorf dir_color = { 0.8, 0.8, 0.8, 1 };
			static struct nk_colorf ambient_color = { 0.2, 0.2, 0.2, 1 };
			static const char* on_off[] = { "Off", "On" };
			//nk_layout_row_static(ctx, 30, GUI_W, 1);
			nk_layout_row_dynamic(ctx, 0, 1);
			if (nk_checkbox_label(ctx, "Use Blending", &use_blending)) {
				printf("Blending %s\n", on_off[use_blending]);
			}

			nk_layout_row_template_begin(ctx, 0);
			nk_layout_row_template_push_static(ctx, 50);
			nk_layout_row_template_push_dynamic(ctx);
			nk_layout_row_template_end(ctx);
			nk_label(ctx, "Alpha", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", 0.0, &uAlpha, 1.0, 0.25, 0.08333);

			nk_layout_row_dynamic(ctx, 0, 1);
			if (nk_checkbox_label(ctx, "Use Lighting", &uUseLighting)) {
				printf("Lighting %s\n", on_off[uUseLighting]);
			}

			nk_label(ctx, "Directional Light", NK_TEXT_CENTERED);
			nk_label(ctx, "Direction", NK_TEXT_CENTERED);

			//nk_layout_row_dynamic(ctx, 0, 2);

			nk_layout_row_template_begin(ctx, 0);
			nk_layout_row_template_push_static(ctx, 20);
			nk_layout_row_template_push_dynamic(ctx);
			nk_layout_row_template_end(ctx);

			nk_label(ctx, "X", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -1.0, &uLightingDirection.x, 1.0, 0.25, 0.08333);
			nk_label(ctx, "Y", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -1.0, &uLightingDirection.y, 1.0, 0.25, 0.08333);
			nk_label(ctx, "Z", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -1.0, &uLightingDirection.z, 1.0, 0.25, 0.08333);

			//nk_layout_row_static(ctx, 30, GUI_W, 1);
			nk_layout_row_dynamic(ctx, 0, 1);

			nk_label(ctx, "Color", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 100, 1);
			dir_color = nk_color_picker(ctx, dir_color, NK_RGB);
			nk_layout_row_dynamic(ctx, 0, 1);
			dir_color.r = nk_propertyf(ctx, "#R:", 0, dir_color.r, 1.0f, 0.01f,0.005f);
			dir_color.g = nk_propertyf(ctx, "#G:", 0, dir_color.g, 1.0f, 0.01f,0.005f);
			dir_color.b = nk_propertyf(ctx, "#B:", 0, dir_color.b, 1.0f, 0.01f,0.005f);

			uDirectionalColor = vec3(dir_color.r, dir_color.g, dir_color.b);

			nk_label(ctx, "Ambient Light", NK_TEXT_CENTERED);
			nk_label(ctx, "Color", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 100, 1);
			ambient_color = nk_color_picker(ctx, ambient_color, NK_RGB);
			nk_layout_row_dynamic(ctx, 0, 1);
			ambient_color.r = nk_propertyf(ctx, "#R:", 0, ambient_color.r, 1.0f, 0.01f,0.005f);
			ambient_color.g = nk_propertyf(ctx, "#G:", 0, ambient_color.g, 1.0f, 0.01f,0.005f);
			ambient_color.b = nk_propertyf(ctx, "#B:", 0, ambient_color.b, 1.0f, 0.01f,0.005f);

			uAmbientColor = vec3(ambient_color.r, ambient_color.g, ambient_color.b);
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

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vert_buf);
	glDeleteBuffers(1, &normal_buf);
	glDeleteBuffers(1, &tex_buf);
	glDeleteBuffers(1, &elem_buf);
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

	window = SDL_CreateWindow("Lesson 8", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
				y_speed -= 1;
			} else if (sc == SDL_SCANCODE_RIGHT) {
				y_speed += 1;
			} else if (sc == SDL_SCANCODE_UP) {
				x_speed -= 1;
			} else if (sc == SDL_SCANCODE_DOWN) {
				x_speed += 1;
			}
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

	x_rot += x_speed*elapsed / 1000.0f;
	y_rot += y_speed*elapsed / 1000.0f;
	return 0;
}





