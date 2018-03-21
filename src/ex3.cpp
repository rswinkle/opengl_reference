#include <gltools.h>
#include <rsw_primitives.h>
#include <rsw_halfedge.h>


#include <SDL2/SDL.h>

#include <stdio.h>
#include <vector>

#define WIDTH 640
#define HEIGHT 480

using namespace std;

using rsw::vec3;
using rsw::vec4;
using rsw::mat3;
using rsw::mat4;


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events();



int load_model(const char* filename, vector<vec3>& verts, vector<ivec3>&tris);


struct vert_attribs
{
	vec3 pos;
	vec2 tex;

	vert_attribs(vec3 p, vec2 t) : pos(p), tex(t) {}
};

int polygon_mode;
int cur_prog;

int mvp_loc;

int main(int argc, char** argv)
{
	setup_context();

	polygon_mode = 2;
	cur_prog = 0;

	vector<vec3> verts;
	vector<ivec3> tris;
	vector<vec2> tex;
	vector<vert_attribs> vert_data;
	int ret;
	vec3 tmp;


	generate_box(verts, tris, tex, 6, 3, 1.5);

	int v;
	for (int i=0, j=0; i<tris.size(); ++i, j=i*3) {
		v = tris[i].x;
		vert_data.push_back(vert_attribs(verts[v], tex[j]));

		v = tris[i].y;
		vert_data.push_back(vert_attribs(verts[v], tex[j+1]));

		v = tris[i].z;
		vert_data.push_back(vert_attribs(verts[v], tex[j+2]));
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (!load_texture2D("../media/textures/test1.jpg", GL_NEAREST, GL_NEAREST, GL_MIRRORED_REPEAT, GL_FALSE)) {
		printf("failed to load texture\n");
		return 0;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size()*sizeof(vert_attribs), &vert_data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_attribs), 0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vert_attribs), (void*)sizeof(vec3));


	glBindVertexArray(0);

	const char vs_file[] = "../media/shaders/texturing.vp";
	const char fs_file[] = "../media/shaders/texturing.fp";

	GLuint program = load_shader_file_pair(vs_file, fs_file);
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}
	glUseProgram(program);

	mat4 proj_mat;
	mat4 view_mat;

	rsw::make_perspective_matrix(proj_mat, DEG_TO_RAD(45), WIDTH/(float)HEIGHT, 1.0f, 100.0f);
	rsw::lookAt(view_mat, vec3(0, 5, 14), vec3(0, 0, 0), vec3(0, 1, 0));

	mat4 mvp_mat;
	mat4 vp_mat = proj_mat * view_mat;
	mat4 rot_mat(1);

	mvp_loc = glGetUniformLocation(program, "mvp_mat");


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

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

		rsw::load_rotation_mat4(rot_mat, vec3(0, 1, 0), 0.5f*new_time/1000.0f);

		mvp_mat = vp_mat * rot_mat;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mvp_mat);
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vert_data.size());

		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buffer);
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

	window = SDL_CreateWindow("Ex 3", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
		}
	}
	return 0;
}




int load_model(const char* filename, vector<vec3>& verts, vector<ivec3>&tris)
{
	FILE* file = NULL;
	unsigned int num = 0;
	vec3 vec;
	ivec3 ivec;

	if (!(file = fopen(filename, "r")))
		return 0;

	fscanf(file, "%u", &num);
	if (!num)
		return 0;

	printf("%u vertices\n", num);
	
	verts.reserve(num);
	for (int i=0; i<num; ++i) {
		fscanf(file, " (%f, %f, %f)", &vec.x, &vec.y, &vec.z);
		verts.push_back(vec);
	}

	fscanf(file, "%u", &num);
	if (!num)
		return 0;

	printf("%u triangles\n", num);
	
	tris.reserve(num);

	for (int i=0; i<num; ++i) {
		fscanf(file, " (%d, %d, %d)", &ivec.x, &ivec.y, &ivec.z);
		tris.push_back(ivec);
	}

	fclose(file);

	return 1;
}








