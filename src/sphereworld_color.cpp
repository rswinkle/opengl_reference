#include <rsw_primitives.h>
#include <gltools.h>

#include <rsw_halfedge.h>
#include <rsw_glframe.h>



#include <SDL2/SDL.h>

#include <stdio.h>

#include <iostream>
#include <vector>

#define WIDTH 640
#define HEIGHT 480


using namespace std;

using rsw::vec2;
using rsw::vec3;
using rsw::vec4;
using rsw::mat3;
using rsw::mat4;


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

	make_torus(torus.verts, torus.tris, torus.texcoords, 0.3, 0.1, 40, 20);
	make_sphere(sphere.verts, sphere.tris, sphere.texcoords, 0.1, 26, 13);

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
		rand_pos = vec2(rsw::rand_float(-FLOOR_SIZE/2.0f, FLOOR_SIZE/2.0f), rsw::rand_float(-FLOOR_SIZE/2.0f, FLOOR_SIZE/2.0f));
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






	GLuint basic_shader = load_shader_file_pair("../media/shaders/basic_transform.vp", "../media/shaders/uniform_color.fp");

	GLuint gouraud_shader = load_shader_file_pair("../media/shaders/gouraud_ads.vp", "../media/shaders/gouraud_ads.fp");

	GLuint phong_shader = load_shader_file_pair("../media/shaders/phong_ads.vp", "../media/shaders/phong_ads.fp");



	mat4 proj_mat;
	mat4 view_mat;
	mat4 mvp_mat;
	mat3 normal_mat;
	mat4 translate_sphere = rsw::translation_mat4(vec3(0.8f, 0.4f, 0.0f));

	rsw::make_perspective_matrix(proj_mat, DEG_TO_RAD(35.0f), WIDTH/(float)HEIGHT, 0.3f, 100.0f);



	vec4 floor_color(0, 1, 0, 1);

	vec3 torus_ambient(0.0, 0, 0);
	vec3 torus_diffuse(1.0, 0, 0);
	vec3 torus_specular(0, 0, 0);

	vec3 sphere_ambient(0, 0, 0.2);
	vec3 sphere_diffuse(0, 0, 0.7);
	vec3 sphere_specular(1, 1, 1);


	glUseProgram(basic_shader);
	set_uniform4fv(basic_shader, "color", (float*)&floor_color);

	glUseProgram(gouraud_shader);

	glUseProgram(phong_shader);
	set_uniform1f(phong_shader, "shininess", 128.0f);

	vec3 light_direction(0, 10, 5);


	glUseProgram(basic_shader);

	GLFrame camera(true);


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	SDL_SetRelativeMouseMode(SDL_TRUE);

	unsigned int old_time = 0, new_time=0, counter = 0, last_time = SDL_GetTicks();
	float total_time;
	while (1) {
		new_time = SDL_GetTicks();
		if (handle_events(camera, last_time, new_time))
			break;

		last_time = new_time;
		total_time = new_time/1000.0f;
		if (new_time - old_time > 3000) {
			printf("%f FPS\n", counter*1000.f/(new_time-old_time));
			old_time = new_time;
			counter = 0;
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


		view_mat = camera.get_camera_matrix();
		mvp_mat = proj_mat * view_mat;


		glUseProgram(basic_shader);
		set_uniform_mat4f(basic_shader, "mvp_mat", (float*)&mvp_mat);
		glBindVertexArray(line_vao);
		glDrawArrays(GL_LINES, 0, line_verts.size());

		

		glBindVertexArray(vao);

		glUseProgram(phong_shader);


		vec3 light_dir = mat3(view_mat)*light_direction;
		set_uniform3fv(phong_shader, "light_direction", (float*)&light_dir);

		set_uniform_mat4f(phong_shader, "mvp_mat", (float*)&mvp_mat);
		normal_mat = mat3(view_mat);
		set_uniform_mat3f(phong_shader, "normal_mat", (float*)&normal_mat);

		set_uniform3fv(phong_shader, "ambient_color", (float*)&sphere_ambient);
		set_uniform3fv(phong_shader, "diffuse_color", (float*)&sphere_diffuse);
		set_uniform3fv(phong_shader, "spec_color", (float*)&sphere_specular);

		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, torus.tris.size()*3, sphere.tris.size()*3, NUM_SPHERES, 1);

		mat4 rot_mat;
		rsw::load_rotation_mat4(rot_mat, vec3(0, 1, 0), -1*total_time*DEG_TO_RAD(60.0f));

		mvp_mat = mvp_mat * rot_mat * translate_sphere;
		normal_mat = mat3(view_mat*rot_mat);
		set_uniform_mat3f(phong_shader, "normal_mat", (float*)&normal_mat);

		set_uniform_mat4f(phong_shader, "mvp_mat", (float*)&mvp_mat);
		glDrawArrays(GL_TRIANGLES, torus.tris.size()*3, sphere.tris.size()*3);


		//draw rotating torus
		mvp_mat = proj_mat * view_mat;
		rot_mat;
		rsw::load_rotation_mat4(rot_mat, vec3(0, 1, 0), total_time*DEG_TO_RAD(60.0f));
		mvp_mat = mvp_mat * rot_mat;
		set_uniform_mat4f(phong_shader, "mvp_mat", (float*)&mvp_mat);
		normal_mat = mat3(view_mat*rot_mat);
		set_uniform_mat3f(phong_shader, "normal_mat", (float*)&normal_mat);

		set_uniform3fv(phong_shader, "ambient_color", (float*)&torus_ambient);
		set_uniform3fv(phong_shader, "diffuse_color", (float*)&torus_diffuse);
		set_uniform3fv(phong_shader, "spec_color", (float*)&torus_specular);

		glDrawArrays(GL_TRIANGLES, 0, torus.tris.size()*3);


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

	window = SDL_CreateWindow("Sphereworld Color", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
			float degx = event.motion.xrel/20.0f;
			float degy = event.motion.yrel/20.0f;
			
			camera_frame.rotate_local_y(DEG_TO_RAD(-degx));
			camera_frame.rotate_local_x(DEG_TO_RAD(degy));
		} break;

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













