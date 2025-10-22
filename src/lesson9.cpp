#include <gltools.h>
#include <glm_matstack.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

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
#define NUM_STARS 50

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

float zoom = -12;
float tilt = 90;
float spin;

matrix_stack mvMatrixStack;

int do_twinkle = 1; // bool

// uniforms
mat4 uMVMatrix;
mat4 uPMatrix;
int uSampler;
vec3 uColor;

int uMVMatrix_loc;
int uPMatrix_loc;
int uSampler_loc;
int uColor_loc;


// NOTE(rswinkle) I do not like this program structure but I am mostly
// going for a straight port. Maybe later I'll refactor it the way I
// would prefer it
struct Star
{
	float angle;
	float dist;
	float rotationSpeed;
	vec3 color;
	vec3 twinkle;

	Star(float startingDistance, float rotationSpeed)
	{
		angle = 0;
		dist = startingDistance;
		this->rotationSpeed = rotationSpeed;
		printf("dist = %f\nrot_speed = %f\n", dist, rotationSpeed);

		randomize_colors();
	}

	void randomize_colors()
	{
		// TODO try different methods for random colors
		//
		// not sure why sphericalRand isn't working like I expect
		//color = glm::sphericalRand(1);
		//twinkle = glm::sphericalRand(1);
		color = vec3(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX);
		twinkle = vec3(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX);
	}

	// angle, angle, bool, angles in degrees
	void draw(float tilt, float spin, int do_twinkle)
	{
		mvMatrixStack.push();

		// move to star's position
		//
		// these two make it work if you remove tilt
		//mvMatrixStack.rotate(glm::radians(angle), vec3(0, 0, 1));
		//mvMatrixStack.translate(dist, 0, 0);
		
		// with tilt
		mvMatrixStack.rotate(glm::radians(angle), vec3(0, 1, 0));
		mvMatrixStack.translate(dist, 0, 0);

		// rotate back so star is facing viewer
		mvMatrixStack.rotate(glm::radians(-angle), vec3(0, 1, 0));
		mvMatrixStack.rotate(glm::radians(-tilt), vec3(1, 0, 0));

		if (do_twinkle) {
			// Draw a non-rotating star in the alternate "twinkling" color
			glUniform3fv(uColor_loc, 1, glm::value_ptr(twinkle));
        	//glUniformMatrix4fv(uMVMatrix_loc, 1,  false, glm::value_ptr(uMVMatrix));
        	glUniformMatrix4fv(uMVMatrix_loc, 1, false, glm::value_ptr(mvMatrixStack.get_matrix()));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // quad via tri strip
		}

		// all stars spin around z at same rate
		mvMatrixStack.rotate(glm::radians(spin), vec3(0, 0, 1));

		// draw star in main color
		glUniform3fv(uColor_loc, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uMVMatrix_loc, 1, false, glm::value_ptr(uMVMatrix));
		glUniformMatrix4fv(uMVMatrix_loc, 1, false, glm::value_ptr(mvMatrixStack.get_matrix()));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // quad via tri strip

		mvMatrixStack.pop();
	}

	void animate(float elapsed_time)
	{
		float effectiveFPMS = 60/1000.0;
		angle += rotationSpeed * effectiveFPMS * elapsed_time;

		// decrease distance, resetting star to outside of spiral if it's
		// at the center
		dist -= 0.01 * effectiveFPMS * elapsed_time;
		if (dist < 0) {
			dist += 5;
			randomize_colors();
		}
		//printf("angle %f\ndist = %f\n", angle, dist);
		//printf("color = (%f, %f, %f)\n", color.x, color.y, color.z);
		//printf("twinkle = (%f, %f, %f)\n", twinkle.x, twinkle.y, twinkle.z);
	}
};

int main(int argc, char** argv)
{
	setup_context();

	mvMatrixStack.load_identity();

	float vertices[] = {
		-1.0, -1.0,  0.0,
		 1.0, -1.0,  0.0,
		-1.0,  1.0,  0.0,
		 1.0,  1.0,  0.0
	};

	float texcoords[] = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};

	vector<Star> stars;
	for (int i=0; i<NUM_STARS; i++) {
		stars.push_back(Star((i/(float)NUM_STARS) * 5.0, i /(float)NUM_STARS));
	}
	//stars.push_back(Star(1.0f, 0.25));

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// TODO should I flip?
	if (!load_texture2D("../media/textures/star.gif", GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_FALSE, GL_FALSE)) {
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

	GLuint tex_buf;
	glGenBuffers(1, &tex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, tex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	const char vs_file[] = "../media/shaders/lesson9.vp";
	const char fs_file[] = "../media/shaders/lesson9.fp";

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
	uColor_loc = glGetUniformLocation(program, "uColor");

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

		for (int i=0; i<stars.size(); i++) {
			stars[i].animate(elapsed);
		}

		// reset state every frame due to GUI
		//glEnable(GL_CULL_FACE);
		glUseProgram(program);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);

		uPMatrix = glm::perspective(glm::radians(45.0f), WIDTH/(float)HEIGHT, 0.1f, 100.0f);
		//uMVMatrix = glm::translate(mat(1), vec3(0, 0, zoom));
		//uMVMatrix = glm::rotate(uMVMatrix, glm::radians(tilt), vec3(1,0,0));
		mvMatrixStack.load_identity();
		mvMatrixStack.translate(0, 0, zoom);
		mvMatrixStack.rotate(glm::radians(tilt), vec3(1, 0, 0));


		glUniformMatrix4fv(uPMatrix_loc, 1, GL_FALSE, glm::value_ptr(uPMatrix));

		//glUniformMatrix4fv(uMVMatrix_loc, 1, GL_FALSE, glm::value_ptr(uMVMatrix));

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);

		for (int i=0; i<stars.size(); i++) {
			stars[i].draw(tilt, spin, do_twinkle);
			spin += 0.1;
		}

		if (nk_begin(ctx, "Controls", nk_rect(WIDTH-GUI_W, 0, GUI_W, HEIGHT),
		    NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
		{
			static const char* on_off[] = { "Off", "On" };
			//nk_layout_row_static(ctx, 30, GUI_W, 1);
			nk_layout_row_dynamic(ctx, 0, 1);
			if (nk_checkbox_label(ctx, "Twinkle", &do_twinkle)) {
				printf("Twinkle %s\n", on_off[do_twinkle]);
			}
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

	window = SDL_CreateWindow("Lesson 9", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
				//y_speed -= 1;
			} else if (sc == SDL_SCANCODE_RIGHT) {
				//y_speed += 1;
			} else if (sc == SDL_SCANCODE_UP) {
				tilt += 2;
				printf("tilt = %f\n", tilt);
			} else if (sc == SDL_SCANCODE_DOWN) {
				tilt -= 2;
				printf("tilt = %f\n", tilt);
			}
		}
		nk_sdl_handle_event(&e);
	}

	//SDL_PumpEvents() is called above in SDL_PollEvent()
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	// TODO change all lessons to use page up/page down like
	// the original
	if (state[SDL_SCANCODE_EQUALS]) {
		zoom += 0.05;
	} else if (state[SDL_SCANCODE_MINUS]) {
		zoom -= 0.05;
	}

	return 0;
}






