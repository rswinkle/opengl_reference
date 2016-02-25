#include <c_utils.h>
#include <primitives.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

//included in SDL.h?
//#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <vector>

#define WIDTH 640
#define HEIGHT 480

using namespace std;

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;


SDL_Window* window;
SDL_GLContext glcontext;

void cleanup();
void setup_context();
int handle_events();


void check_errors(int n=0, const char* str="Errors cleared");
int compile_shader_str(GLuint shader, const char* shader_str);
int link_program(GLuint program);
GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src);
GLuint load_shader_file_pair(const char* vert_file, const char* frag_file);


struct vert_attribs
{
	vec3 pos;
	vec3 normal;

	vert_attribs(vec3 p, vec3 n) : pos(p), normal(n) {}
};

int polygon_mode;


int main(int argc, char** argv)
{
	setup_context();

	polygon_mode = 2;

	//no error checking done for any of this except shader compilation
	GLuint program = load_shader_file_pair("../media/shaders/gouraud_ads.vp", "../media/shaders/gouraud_ads.fp");
	if (!program) {
		printf("failed to compile/link shaders\n");
		exit(0);
	}

	glUseProgram(program);



	vector<vec3> verts;
	vector<ivec3> tris;
	vector<vec2> tex;
	generate_sphere(verts, tris, tex, 2.0f, 14, 7);
	//generate_sphere(verts, tris, tex, 2.0f, 30, 15);

	vector<vert_attribs> vert_data;
	vec3 tmp;
	for (int i=0; i<tris.size(); ++i) {
		tmp = verts[tris[i].x];
		vert_data.push_back(vert_attribs(tmp, normalize(tmp)));
		tmp = verts[tris[i].y];
		vert_data.push_back(vert_attribs(tmp, normalize(tmp)));
		tmp = verts[tris[i].z];
		vert_data.push_back(vert_attribs(tmp, normalize(tmp)));
	}



	//no default vao in core profile ...
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size()*sizeof(vert_attribs), &vert_data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), (void*)sizeof(vec3));


	vec4 material(0.2, 0.6, 1.0, 128.0);
	int loc = glGetUniformLocation(program, "material");
	glUniform4fv(loc, 1, glm::value_ptr(material));

	mat4 proj_mat = glm::perspective(3.14159f/4.0f, WIDTH/(float)HEIGHT, 0.1f, 30.0f);
	mat4 view_mat = glm::lookAt(vec3(0, 0, 10), vec3(0, 0, -1), vec3(0, 1, 0));

	mat4 mvp_mat;
	mat4 vp_mat = proj_mat * view_mat;
	mat3 normal_mat;
	mat4 rot_mat(1);

	int mvp_loc = glGetUniformLocation(program, "mvp_mat");
	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp_mat));

	int normal_loc = glGetUniformLocation(program, "normal_mat");
	glUniformMatrix3fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_mat));


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


		rot_mat = glm::rotate(mat4(1), 0.5f*new_time/1000.0f, vec3(0, 1, 0));

		normal_mat = mat3(view_mat*rot_mat);
		glUniformMatrix3fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_mat));

		mvp_mat = vp_mat * rot_mat;
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp_mat));
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, vert_data.size());

		SDL_GL_SwapWindow(window);
	}

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

	window = SDL_CreateWindow("Ex 1", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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




int load_model(const char* filename, vector<vec3>& verts, vector<ivec3>&tris);
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
		fscanf(file, " (%f, %f, %f)", &vec->x, &vec->y, &vec->z);
		verts.push_back(vec);
	}

	fscanf(file, "%u", &num);
	if (!num)
		return 0;

	printf("%u triangles\n", num);
	
	tris.reserve(num);

	for (int i=0; i<num; ++i) {
		fscanf(f, " (%d, %d, %d)", &ivec->x, &ivec->y, &ivec->z);
		tris.push_back(ivec);
	}

	fclose(file);

	return 1;
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

	return 1;
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






