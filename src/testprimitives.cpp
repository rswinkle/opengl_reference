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


struct vert_attribs
{
	vec3 pos;
	vec3 normal;

	vert_attribs(vec3 p, vec3 n) : pos(p), normal(n) {}
};

// lots of data duplication here but it's just a quick demo
struct mesh
{
	vector<vec3> verts;
	vector<ivec3> tris;
	vector<vec2> tex;
	vector<vec3> normals;

	vector<vec3> draw_verts;

	vector<vert_attribs> vert_data;
	GLuint vao;
	GLuint buffer;

	vector<vec3> normal_lines;
	GLuint normal_vao;
	GLuint norm_buf;

};

enum {
	BOX,
	CYLINDER,
	PLANE,
	SPHERE,
	TORUS,
	CONE,
	TETRA,
	CUBE,
	OCTA,
	DODECA,
	ICOSA,
	NUM_SHAPES
};

int polygon_mode;
int cur_prog;
int cur_shape;
bool show_normals;

#define NUM_PROGRAMS 2
int programs[NUM_PROGRAMS];

const char* shader_files[] =
{
"../media/shaders/gouraud_ads_grayscale.vp",
"../media/shaders/gouraud_ads.fp",
"../media/shaders/phong_ads.vp",
"../media/shaders/phong_ads_grayscale.fp"
};

int mvp_loc, normal_loc;

int main(int argc, char** argv)
{
	setup_context();

	polygon_mode = 2;
	cur_prog = 0;

	mesh shapes[NUM_SHAPES];

	vector<vec3> verts;
	vector<ivec3> tris;
	vector<vec2> tex;
	vector<vert_attribs> vert_data;
	vec3 tmp;

	make_box(shapes[BOX].verts, shapes[BOX].tris, shapes[BOX].tex, 2.0f, 2.0f, 2.0f, true, ivec3(4, 4, 4), vec3(0,0,0));
	make_cylinder(shapes[CYLINDER].verts, shapes[CYLINDER].tris, shapes[CYLINDER].tex, 1, 2, 30);
	make_plane(shapes[PLANE].verts, shapes[PLANE].tris, shapes[PLANE].tex, vec3(-2, -2, 0), vec3(0,1,0), vec3(1,0,0), 4, 4, false);
	make_sphere(shapes[SPHERE].verts, shapes[SPHERE].tris, shapes[SPHERE].tex, 2.0f, 30, 15);
	make_torus(shapes[TORUS].verts, shapes[TORUS].tris, shapes[TORUS].tex, 2.0f, 0.6f, 40, 20);
	make_cone(shapes[CONE].verts, shapes[CONE].tris, shapes[CONE].tex, 1, 2, 30, 1);

	make_tetrahedron(shapes[TETRA].verts, shapes[TETRA].tris);
	make_cube(shapes[CUBE].verts, shapes[CUBE].tris);
	make_octahedron(shapes[OCTA].verts, shapes[OCTA].tris);
	make_dodecahedron(shapes[DODECA].verts, shapes[DODECA].tris);
	make_icosahedron(shapes[ICOSA].verts, shapes[ICOSA].tris);

	vector<vec3> draw_verts;
	for (int j=0; j<NUM_SHAPES; j++) {
		mesh& s = shapes[j];

		expand_verts(s.draw_verts, s.verts, s.tris);

		compute_face_normals(s.verts, s.tris, s.normals);

		for (int i=0; i<s.draw_verts.size(); i+=3) {
			s.vert_data.push_back(vert_attribs(s.draw_verts[i],   s.normals[i]));
			s.vert_data.push_back(vert_attribs(s.draw_verts[i+1], s.normals[i]));
			s.vert_data.push_back(vert_attribs(s.draw_verts[i+2], s.normals[i]));

			tmp = s.draw_verts[i] + s.draw_verts[i+1] + s.draw_verts[i+2];
			tmp /= 3;

			s.normal_lines.push_back(tmp);
			s.normal_lines.push_back(tmp + s.normals[i]*0.5f);
		}

		glGenVertexArrays(1, &s.vao);
		glBindVertexArray(s.vao);

		glGenBuffers(1, &s.buffer);
		glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
		glBufferData(GL_ARRAY_BUFFER, s.vert_data.size()*sizeof(vert_attribs), &s.vert_data[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_attribs), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vert_attribs), (void*)sizeof(vec3));

		glGenVertexArrays(1, &s.normal_vao);
		glBindVertexArray(s.normal_vao);

		glGenBuffers(1, &s.norm_buf);
		glBindBuffer(GL_ARRAY_BUFFER, s.norm_buf);
		glBufferData(GL_ARRAY_BUFFER, s.normal_lines.size()*sizeof(vec3), &s.normal_lines[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


		glBindVertexArray(0);
	}


	int loc;
	GLuint normal_prog = load_shader_file_pair("../media/shaders/basic_transform.vp", "../media/shaders/uniform_color.fp");
	glUseProgram(normal_prog);

	vec4 Red(1.0, 0.0, 0.0, 1.0);
	loc = glGetUniformLocation(normal_prog, "color");
	glUniform4fv(loc, 1, (float*)&Red);



	vec4 material(0.2, 0.6, 1.0, 128.0);
	//no error checking done for any of this except shader compilation
	for (int i=0; i<NUM_PROGRAMS; ++i) {
		programs[i] = load_shader_file_pair(shader_files[i*2], shader_files[i*2+1]);
		if (!programs[i]) {
			printf("failed to compile/link shaders\n");
			exit(0);
		}

		glUseProgram(programs[i]);
		loc = glGetUniformLocation(programs[i], "material");
		glUniform4fv(loc, 1, (float*)&material);
	}

	glUseProgram(programs[cur_prog]);

	mat4 proj_mat;
	mat4 view_mat;

	rsw::make_perspective_matrix(proj_mat, DEG_TO_RAD(45), WIDTH/(float)HEIGHT, 1.0f, 100.0f);
	rsw::lookAt(view_mat, vec3(0, 3, 10), vec3(0, 0, 0), vec3(0, 1, 0));

	mat4 mvp_mat;
	mat4 vp_mat = proj_mat * view_mat;
	mat3 normal_mat;
	mat4 rot_mat(1);

	mvp_loc = glGetUniformLocation(programs[cur_prog], "mvp_mat");
	normal_loc = glGetUniformLocation(programs[cur_prog], "normal_mat");


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

		glUseProgram(programs[cur_prog]);

		rsw::load_rotation_mat4(rot_mat, vec3(0, 1, 0), 0.5f*new_time/1000.0f);

		normal_mat = mat3(view_mat*rot_mat);
		glUniformMatrix3fv(normal_loc, 1, GL_FALSE, (float*)&normal_mat);

		mvp_mat = vp_mat * rot_mat;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)&mvp_mat);
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(shapes[cur_shape].vao);
		glDrawArrays(GL_TRIANGLES, 0, shapes[cur_shape].vert_data.size());

		if (show_normals) {
			glUseProgram(normal_prog);
			loc = glGetUniformLocation(normal_prog, "mvp_mat");
			glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&mvp_mat);

			glBindVertexArray(shapes[cur_shape].normal_vao);
			glDrawArrays(GL_LINES, 0, shapes[cur_shape].normal_lines.size());
		}

		SDL_GL_SwapWindow(window);
	}

	for (int i=0; i<NUM_SHAPES; i++) {
		glDeleteVertexArrays(1, &shapes[i].vao);
		glDeleteBuffers(1, &shapes[i].buffer);

		glDeleteVertexArrays(1, &shapes[i].normal_vao);
		glDeleteBuffers(1, &shapes[i].norm_buf);
	}
	glDeleteProgram(programs[cur_prog]);

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

	window = SDL_CreateWindow("Test Primitives", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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
			} else if (sc == SDL_SCANCODE_S) {
				cur_prog = (cur_prog + 1) % NUM_PROGRAMS;

				glUseProgram(programs[cur_prog]);
				mvp_loc = glGetUniformLocation(programs[cur_prog], "mvp_mat");
				normal_loc = glGetUniformLocation(programs[cur_prog], "normal_mat");
			} else if (sc == SDL_SCANCODE_RIGHT) {
				cur_shape = (cur_shape + 1) % NUM_SHAPES;
			} else if (sc == SDL_SCANCODE_LEFT) {
				cur_shape--;
				if (cur_shape < 0) {
					cur_shape = NUM_SHAPES-1;
				}
			} else if (sc == SDL_SCANCODE_N) {
				show_normals = !show_normals;
			}
		}
	}
	return 0;
}




