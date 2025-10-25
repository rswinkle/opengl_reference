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
int setup_buffers();

float z;
float x_rot, y_rot;
float x_speed, y_speed;

GLuint vao;

// uniforms
mat4 uMVMatrix;
mat4 uPMatrix;
mat3 uNMatrix;
vec3 uAmbientColor(0.2);
vec3 uPntLight(-10.0, 4.0, -20.0);
vec3 uPntLightSpec(0.8);
vec3 uPntLightDiff(0.8);
float uShininess = 32;
int uUseLighting = 1;
int uShowSpecular = 1;
int uUseTextures = 1;
int uSampler;

int uMVMatrix_loc;
int uPMatrix_loc;
int uNMatrix_loc;
int uAmbientColor_loc;
int uPntLight_loc;
int uPntLightSpec_loc;
int uPntLightDiff_loc;

int uUseLighting_loc;
int uUseTextures_loc;
int uShowSpecular_loc;
int uShininess_loc;
int uSampler_loc;


int main(int argc, char** argv)
{
	setup_context();

	GLuint earth_tex, galvanized_tex;
	glGenTextures(1, &earth_tex);
	glBindTexture(GL_TEXTURE_2D, earth_tex);
	if (!load_texture2D("../media/textures/earth.jpg", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_REPEAT, GL_TRUE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}
	glGenTextures(1, &galvanized_tex);
	glBindTexture(GL_TEXTURE_2D, galvanized_tex);
	if (!load_texture2D("../media/textures/arroway.de_metal+structure+06_d100_flat.jpg", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_REPEAT, GL_TRUE, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	int num_teapot_indices = setup_buffers();

	glBindVertexArray(0);

	const char vs_file[] = "../media/shaders/lesson14.vp";
	const char fs_file[] = "../media/shaders/lesson14.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(program);

	z = -5;

	uMVMatrix_loc = glGetUniformLocation(program, "uMVMatrix");
	uPMatrix_loc = glGetUniformLocation(program, "uPMatrix");
	uNMatrix_loc = glGetUniformLocation(program, "uNMatrix");
	uAmbientColor_loc = glGetUniformLocation(program, "uAmbientColor");

	uPntLight_loc = glGetUniformLocation(program, "uPntLight");
	uPntLightSpec_loc = glGetUniformLocation(program, "uPntLightSpec");
	uPntLightDiff_loc = glGetUniformLocation(program, "uPntLightDiff");

	uUseLighting_loc = glGetUniformLocation(program, "uUseLighting");
	uUseTextures_loc = glGetUniformLocation(program, "uUseTextures");
	uShowSpecular_loc = glGetUniformLocation(program, "uShowSpecular");
	uShininess_loc = glGetUniformLocation(program, "uShininess");
	uSampler_loc = glGetUniformLocation(program, "uSampler");

	check_errors(0, "uniform locs");

	glUniform1i(uSampler_loc, 0);

	int which_texture = 1;
	float teapotAngle = 180;
	glBindTexture(GL_TEXTURE_2D, 0);

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

		teapotAngle += 0.05 * elapsed;

		// reset state every frame due to GUI
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glUseProgram(program);

		uPMatrix = glm::perspective(glm::radians(45.0f), WIDTH/(float)HEIGHT, 0.1f, 100.0f);
		uMVMatrix = glm::translate(mat4(1), vec3(0,0,-40));
		uMVMatrix = glm::rotate(uMVMatrix, glm::radians(23.4f), vec3(1, 0, -1));
		uMVMatrix = glm::rotate(uMVMatrix, glm::radians(teapotAngle), vec3(0, 1, 0));

		glUniformMatrix4fv(uMVMatrix_loc, 1, GL_FALSE, glm::value_ptr(uMVMatrix));
		glUniformMatrix4fv(uPMatrix_loc, 1, GL_FALSE, glm::value_ptr(uPMatrix));

		// transpose/inverse not necessary here but meh
		//uNMatrix = mat3(uMVMatrix);
		uNMatrix = glm::transpose(glm::inverse(mat3(uMVMatrix)));
		glUniformMatrix3fv(uNMatrix_loc, 1, GL_FALSE, glm::value_ptr(uNMatrix));

		glUniform3fv(uAmbientColor_loc, 1, glm::value_ptr(uAmbientColor));
		glUniform3fv(uPntLight_loc, 1, glm::value_ptr(uPntLight));
		glUniform3fv(uPntLightSpec_loc, 1, glm::value_ptr(uPntLightSpec));
		glUniform3fv(uPntLightDiff_loc, 1, glm::value_ptr(uPntLightDiff));

		glUniform1f(uShininess_loc, uShininess);
		glUniform1i(uUseLighting_loc, uUseLighting);
		glUniform1i(uUseTextures_loc, uUseTextures);
		glUniform1i(uShowSpecular_loc, uShowSpecular);

		if (which_texture == 1) {
			glBindTexture(GL_TEXTURE_2D, galvanized_tex);
		} else if (which_texture == 2) {
			glBindTexture(GL_TEXTURE_2D, earth_tex);
		//} else {
		//	glBindTexture(GL_TEXTURE_2D, 0);
		}


		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, num_teapot_indices, GL_UNSIGNED_INT, 0);


		if (nk_begin(ctx, "Controls", nk_rect(WIDTH-GUI_W, 0, GUI_W, HEIGHT),
		    NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
		{
			static struct nk_colorf spec_color = { 0.8, 0.8, 0.8, 1 };
			static struct nk_colorf diff_color = { 0.8, 0.8, 0.8, 1 };
			static struct nk_colorf ambient_color = { 0.2, 0.2, 0.2, 1 };
			static const char* texture_opts[] = { "None", "Galvanized", "Earth" };
			//nk_layout_row_static(ctx, 30, GUI_W, 1);
			nk_layout_row_dynamic(ctx, 0, 1);
			if (nk_checkbox_label(ctx, "Show Specular", &uShowSpecular)) {
				printf("Lighting %s\n", (uShowSpecular ? "On" : "Off"));
			}
			if (nk_checkbox_label(ctx, "Use Lighting", &uUseLighting)) {
				printf("Lighting %s\n", (uUseLighting ? "On" : "Off"));
			}

			nk_label(ctx, "Texture:", NK_TEXT_LEFT);
			// TODO look up nuklear default font size
			struct nk_rect bounds = nk_widget_bounds(ctx);
			which_texture = nk_combo(ctx, texture_opts, NK_LEN(texture_opts), which_texture, 11, nk_vec2(bounds.w, 300));
			uUseTextures = !!which_texture;

			nk_label(ctx, "Material", NK_TEXT_CENTERED);
			nk_label(ctx, "Shininess", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", 0, &uShininess, 1024, 1, 0.1);

			nk_label(ctx, "Point Light", NK_TEXT_CENTERED);
			nk_label(ctx, "Location", NK_TEXT_CENTERED);

			//nk_layout_row_dynamic(ctx, 0, 2);

			nk_layout_row_template_begin(ctx, 0);
			nk_layout_row_template_push_static(ctx, 20);
			nk_layout_row_template_push_dynamic(ctx);
			nk_layout_row_template_end(ctx);

			nk_label(ctx, "X", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -20.0, &uPntLight.x, 20.0, 0.25, 0.08333);
			nk_label(ctx, "Y", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -20.0, &uPntLight.y, 20.0, 0.25, 0.08333);
			nk_label(ctx, "Z", NK_TEXT_LEFT);
			nk_property_float(ctx, "#", -20.0, &uPntLight.z, 20.0, 0.25, 0.08333);

			//nk_layout_row_static(ctx, 30, GUI_W, 1);
			nk_layout_row_dynamic(ctx, 0, 1);

			nk_label(ctx, "Specular Color", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 100, 1);
			spec_color = nk_color_picker(ctx, spec_color, NK_RGB);
			nk_layout_row_dynamic(ctx, 0, 1);
			spec_color.r = nk_propertyf(ctx, "#R:", 0, spec_color.r, 1.0f, 0.01f,0.005f);
			spec_color.g = nk_propertyf(ctx, "#G:", 0, spec_color.g, 1.0f, 0.01f,0.005f);
			spec_color.b = nk_propertyf(ctx, "#B:", 0, spec_color.b, 1.0f, 0.01f,0.005f);
			uPntLightSpec = vec3(spec_color.r, spec_color.g, spec_color.b);

			nk_label(ctx, "Diffuse Color", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 100, 1);
			diff_color = nk_color_picker(ctx, diff_color, NK_RGB);
			nk_layout_row_dynamic(ctx, 0, 1);
			diff_color.r = nk_propertyf(ctx, "#R:", 0, diff_color.r, 1.0f, 0.01f,0.005f);
			diff_color.g = nk_propertyf(ctx, "#G:", 0, diff_color.g, 1.0f, 0.01f,0.005f);
			diff_color.b = nk_propertyf(ctx, "#B:", 0, diff_color.b, 1.0f, 0.01f,0.005f);

			uPntLightDiff = vec3(diff_color.r, diff_color.g, diff_color.b);

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

	window = SDL_CreateWindow("Lesson 14", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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


// TODO convert teapot to text file
// or download it in another format and convert
int setup_buffers()
{
#include "../media/models/teapot.h"

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vert_buf;
	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint normal_buf;
	glGenBuffers(1, &normal_buf);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals), vertexNormals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tex_buf;
	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTextureCoords), vertexTextureCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint elem_buf;
	glGenBuffers(1, &elem_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	return sizeof(indices)/sizeof(indices[0]);
}


