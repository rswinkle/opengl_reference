#include <primitives.h>
#include <gltools.h> //brings in glew.h and gl.h

#include <glm_halfedge.h>

#include <glm_glframe.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>


#include <SDL2/SDL.h>

#include <stdio.h>

#include <iostream>
#include <vector>

#define WIDTH 640
#define HEIGHT 480

//TODO
#define RM_PI (3.14159265358979323846)
#define RM_2PI (2.0 * RM_PI)
#define PI_DIV_180 (0.017453292519943296f)
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





struct Mesh
{
	vector<vec3> verts;
	vector<ivec3> tris;
	vector<vec3> normals;

	vector<vec2> texcoords;

	half_edge_data he_data;
};


struct vert_attribs
{
	vec3 pos;
	vec3 normal;

	vert_attribs(vec3 p, vec3 n) : pos(p), normal(n) {}
};


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


#define FLOOR_SIZE 40



int main(int argc, char** argv)
{
	setup_context();

	polygon_mode = 2;



	vector<vec3> line_verts;
	for (int i=0, j=-FLOOR_SIZE/2; i < 11; ++i, j+=FLOOR_SIZE/10) {
		line_verts.push_back(vec3(j, -1, -FLOOR_SIZE/2));
		line_verts.push_back(vec3(j, -1, FLOOR_SIZE/2));
		line_verts.push_back(vec3(-FLOOR_SIZE/2, -1, j));
		line_verts.push_back(vec3(FLOOR_SIZE/2, -1, j));
	}

	GLuint line_vao, line_buf;
	glGenVertexArrays(1, &line_vao);
	glBindVertexArray(line_vao);

	glGenBuffers(1, &line_buf);
	glBindBuffer(GL_ARRAY_BUFFER, line_buf);
	glBufferData(GL_ARRAY_BUFFER, line_verts.size()*3*sizeof(float), &line_verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	Mesh torus;
	Mesh sphere;

	generate_torus(torus.verts, torus.tris, torus.texcoords, 0.4, 0.15, 30, 30);
	generate_sphere(sphere.verts, sphere.tris, sphere.texcoords, 0.1, 26, 13);

	compute_normals(torus.verts, torus.tris, NULL, DEG_TO_RAD(30), torus.normals);
	compute_normals(sphere.verts, sphere.tris, NULL, DEG_TO_RAD(30), sphere.normals);


	vector<vert_attribs> vert_data;

	int v;
	for (int i=0, j=0; i<torus.tris.size(); ++i, j+=3) {
		v = torus.tris[i].x;
		vert_data.push_back(vert_attribs(torus.verts[v], torus.normals[j]));

		v = torus.tris[i].y;
		vert_data.push_back(vert_attribs(torus.verts[v], torus.normals[j+1]));

		v = torus.tris[i].z;
		vert_data.push_back(vert_attribs(torus.verts[v], torus.normals[j+2]));
	}

	for (int i=0, j=0; i<sphere.tris.size(); ++i, j+=3) {
		v = sphere.tris[i].x;
		vert_data.push_back(vert_attribs(sphere.verts[v], sphere.normals[j]));

		v = sphere.tris[i].y;
		vert_data.push_back(vert_attribs(sphere.verts[v], sphere.normals[j+1]));

		v = sphere.tris[i].z;
		vert_data.push_back(vert_attribs(sphere.verts[v], sphere.normals[j+2]));
	}

#define NUM_SPHERES 50
	vector<vec3> instance_pos;
	vec2 rand_pos;
	for (int i=0; i<NUM_SPHERES+1; ++i) {
		rand_pos = glm::diskRand(FLOOR_SIZE/2.0f);
		if (i)
			instance_pos.push_back(vec3(rand_pos.x, 0.4, rand_pos.y));
		else
			instance_pos.push_back(vec3());
	}




	GLuint vao, buffer;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	size_t total_size = (torus.tris.size()*3 + sphere.tris.size()*3) * sizeof(vert_attribs);
	size_t sphere_offset = torus.tris.size()*3;
	glBufferData(GL_ARRAY_BUFFER, total_size, &vert_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_attribs), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vert_attribs), (void*)sizeof(vec3));

	GLuint inst_buf;
	glGenBuffers(1, &inst_buf);
	glBindBuffer(GL_ARRAY_BUFFER, inst_buf);
	glBufferData(GL_ARRAY_BUFFER, instance_pos.size()*3*sizeof(float), &instance_pos[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(3, 1);






	GLuint basic_shader = load_shader_file_pair("../media/shaders/basic_transform.vp", "../media/shaders/simple_color.fp");

	GLuint gouraud_shader = load_shader_file_pair("../media/shaders/gouraud_ads.vp", "../media/shaders/gouraud_ads.fp");

	GLuint phong_shader = load_shader_file_pair("../media/shaders/phong_ads.vp", "../media/shaders/phong_ads.fp");



	mat4 proj_mat = glm::perspective(DEG_TO_RAD(35.0f), WIDTH/(float)HEIGHT, 1.0f, 100.0f);
	mat4 view_mat;
	mat4 mvp_mat;
	mat3 normal_mat;


	vec4 floor_color(0, 1, 0, 1);

	vec3 torus_ambient(0.0, 0, 0);
	vec3 torus_diffuse(1.0, 0, 0);
	vec3 torus_specular(0, 0, 0);

	vec3 sphere_ambient(0, 0, 0.2);
	vec3 sphere_diffuse(0, 0, 0.7);
	vec3 sphere_specular(1, 1, 1);


	glUseProgram(basic_shader);
	set_uniform4fv(basic_shader, "color", glm::value_ptr(floor_color));

	glUseProgram(gouraud_shader);

	glUseProgram(phong_shader);
	set_uniform1f(phong_shader, "shininess", 128.0f);

	vec3 light_dir(0, 10, 5);
	set_uniform3fv(phong_shader, "light_direction", glm::value_ptr(light_dir));


	glUseProgram(basic_shader);

	GLFrame camera(true);


	glEnable(GL_CULL_FACE);
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


		view_mat = camera.get_camera_matrix();
		mvp_mat = proj_mat * view_mat;


		glUseProgram(basic_shader);
		set_uniform_mat4f(basic_shader, "mvp_mat", glm::value_ptr(mvp_mat));
		glBindVertexArray(line_vao);
		glDrawArrays(GL_LINES, 0, line_verts.size());

		

		glUseProgram(phong_shader);

		set_uniform_mat4f(phong_shader, "mvp_mat", glm::value_ptr(mvp_mat));
		normal_mat = mat3(view_mat);
		set_uniform_mat3f(phong_shader, "normal_mat", glm::value_ptr(normal_mat));

		set_uniform3fv(phong_shader, "ambient_color", glm::value_ptr(torus_ambient));
		set_uniform3fv(phong_shader, "diffuse_color", glm::value_ptr(torus_diffuse));
		set_uniform3fv(phong_shader, "spec_color", glm::value_ptr(torus_specular));

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, torus.tris.size()*3);

		set_uniform3fv(phong_shader, "ambient_color", glm::value_ptr(sphere_ambient));
		set_uniform3fv(phong_shader, "diffuse_color", glm::value_ptr(sphere_diffuse));
		set_uniform3fv(phong_shader, "spec_color", glm::value_ptr(sphere_specular));

		glDrawArraysInstanced(GL_TRIANGLES, torus.tris.size()*3, sphere.tris.size()*3, NUM_SPHERES);


		SDL_GL_SwapWindow(window);

		last_time = new_time;
		++counter;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(basic_shader);

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

	//for now
	int width = WIDTH;
	int height = HEIGHT;

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

#define MOVE_SPEED 5
	
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
		camera_frame.move_forward(time * MOVE_SPEED);
	}
	if (state[controls[BACK]]) {
		camera_frame.move_forward(time * -MOVE_SPEED);
	}
	if (state[controls[TILTLEFT]]) {
		camera_frame.rotate_local_z(DEG_TO_RAD(-60*time));
	}
	if (state[controls[TILTRIGHT]]) {
		camera_frame.rotate_local_z(DEG_TO_RAD(60*time));
	}


	return 0;
}












