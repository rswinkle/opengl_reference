#include <c_utils.h>
#include <primitives.h>

#include <glm_glframe.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>

//included in SDL.h?
//#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <stdio.h>

#include <iostream>
#include <vector>

#define WIDTH 640
#define HEIGHT 480

//TODO
#define RM_PI (3.14159265358979323846)
#define RM_2PI (2.0 * RM_PI)
#define PI_DIV_180 (0.017453292519943296)
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


void check_errors(int n=0, const char* str="Errors cleared");
int compile_shader_str(GLuint shader, const char* shader_str);
int link_program(GLuint program);
GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src);
GLuint load_shader_file_pair(const char* vert_file, const char* frag_file);

int load_model(const char* filename, vector<vec3>& verts, vector<ivec3>&tris);
void compute_face_normals(vector<vec3>& verts, vector<ivec3>& tris, vector<vec3>& normals);


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





int main(int argc, char** argv)
{
	setup_context();

	polygon_mode = 2;



	vector<vec3> line_verts;
#define GRID_SIZE 200
	for (int i=0, j=-GRID_SIZE/2; i < 11; ++i, j+=GRID_SIZE/10) {
		line_verts.push_back(vec3(j, -1, -GRID_SIZE/2));
		line_verts.push_back(vec3(j, -1, GRID_SIZE/2));
		line_verts.push_back(vec3(-GRID_SIZE/2, -1, j));
		line_verts.push_back(vec3(GRID_SIZE/2, -1, j));
	}
#undef GRID_SIZE

	line_verts.push_back(vec3(0));
	line_verts.push_back(vec3(2,0,0));
	line_verts.push_back(vec3(0));
	line_verts.push_back(vec3(0,2,0));
	line_verts.push_back(vec3(0));
	line_verts.push_back(vec3(0,0,2));

	GLuint vao, buffer;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, line_verts.size()*3*sizeof(float), &line_verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint prog = load_shader_file_pair("../media/shaders/basic_transform.vp", "../media/shaders/simple_color.fp");




	glUseProgram(prog);

	mat4 proj_mat = glm::perspective(glm::quarter_pi<float>(), WIDTH/(float)HEIGHT, 0.1f, 300.0f);
	mat4 view_mat;

	mat4 mvp_mat;

	int mvp_loc = glGetUniformLocation(prog, "mvp_mat");

	int color_loc = glGetUniformLocation(prog, "color");

	vec4 Red(1, 0, 0, 1);
	vec4 Green(0, 1, 0, 1);
	vec4 Blue(0, 0, 1, 1);

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


		view_mat = glm::lookAt(camera.origin, camera.forward, camera.up);
		//cout << glm::to_string(view_mat) << "\n";
		view_mat = camera.get_camera_matrix();
		//cout << glm::to_string(view_mat) << "\n\n";

		mvp_mat = proj_mat * view_mat;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp_mat));

		

		glUniform4fv(color_loc, 1, glm::value_ptr(Red));
		glDrawArrays(GL_LINES, 0, line_verts.size() - 6);

		glUniform4fv(color_loc, 1, glm::value_ptr(Blue));
		glDrawArrays(GL_LINES, line_verts.size()-6, 6);

		SDL_GL_SwapWindow(window);

		last_time = new_time;
		++counter;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(prog);

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
		camera_frame.move_forward(time*20);
	}
	if (state[controls[BACK]]) {
		camera_frame.move_forward(time*-20);
	}
	if (state[controls[TILTLEFT]]) {
		camera_frame.rotate_local_z(DEG_TO_RAD(-60*time));
	}
	if (state[controls[TILTRIGHT]]) {
		camera_frame.rotate_local_z(DEG_TO_RAD(60*time));
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

void compute_face_normals(vector<vec3>& verts, vector<ivec3>& tris, vector<vec3>& normals)
{
	vec3 v1, v2, tmp;
	for (int i=0; i<tris.size(); ++i) {
		v1 = verts[tris[i].y] - verts[tris[i].x];
		v2 = verts[tris[i].z] - verts[tris[i].x];
		tmp = glm::cross(v1, v2);

		normals.push_back(normalize(tmp));
	}
}




void check_errors(int n, const char* str)
{
	GLenum error;
	int err = 0;
	while ((error = glGetError()) != GL_NO_ERROR) {
		switch (error)
		{
		case GL_INVALID_ENUM:
			fprintf(stderr, "invalid enum\n");
			break;
		case GL_INVALID_VALUE:
			fprintf(stderr, "invalid value\n");
			break;
		case GL_INVALID_OPERATION:
			fprintf(stderr, "invalid operation\n");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			fprintf(stderr, "invalid framebuffer operation\n");
			break;
		case GL_OUT_OF_MEMORY:
			fprintf(stderr, "out of memory\n");
			break;
		default:
			fprintf(stderr, "wtf?\n");
		}
		err = 1;
	}
	if (err)
		fprintf(stderr, "%d: %s\n\n", n, str);
}

#define BUF_SIZE 1000

int link_program(GLuint program)
{
	glLinkProgram(program);
	int status = 0;
	char info_buf[BUF_SIZE];
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		int len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		if (len > 0) {
			int written = 0;
			glGetProgramInfoLog(program, len, &written, info_buf);
			printf("Link failed:\n===============\n%s\n", info_buf);
		}
		return 0;
	}

	return program;
}

int compile_shader_str(GLuint shader, const char* shader_str)
{
	glShaderSource(shader, 1, &shader_str, NULL);
	glCompileShader(shader);

	int result;
	char shader_info_buf[BUF_SIZE];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		int length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length > 0 && length <= BUF_SIZE) {
			int written = 0;
			glGetShaderInfoLog(shader, BUF_SIZE, &written, shader_info_buf);

			printf("Compile failed:\n===============\n%s\n", shader_info_buf);
		}
		return 0;
	}
	return 1;
}

GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src)
{
	GLuint program, vert_shader, frag_shader;

	program = glCreateProgram();
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	if (!compile_shader_str(vert_shader, vert_shader_src))
		return 0;
	if (!compile_shader_str(frag_shader, frag_shader_src))
		return 0;

	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return link_program(program);
}

GLuint load_shader_file_pair(const char* vert_file, const char* frag_file)
{
	c_array vs_str, fs_str;

	if (!file_open_read(vert_file, "r", &vs_str))
		return 0;
	if (!file_open_read(frag_file, "r", &fs_str)) {
		free(vs_str.data);
		return 0;
	}

	return load_shader_pair((char*)vs_str.data, (char*)fs_str.data);
}







