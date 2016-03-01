#include <GL/glew.h>
#include <GL/gl.h>

// Universal includes
#include <stdio.h>
#include <math.h>


//general
void check_errors(int n, const char* str);




//shaders
int compile_shader_str(GLuint shader, const char* shader_str);
int link_program(GLuint program);
GLuint load_shader_pair(const char* vert_shader_src, const char* frag_shader_src);
GLuint load_shader_file_pair(const char* vert_file, const char* frag_file);

void set_uniform1f(GLuint program, const char* name, float val);
void set_uniform2f(GLuint program, const char* name, float x, float y);
void set_uniform3f(GLuint program, const char* name, float x, float y, float z);
void set_uniform4f(GLuint program, const char* name, float x, float y, float z, float w);

void set_uniform2fv(GLuint program, const char* name, GLfloat* v);
void set_uniform3fv(GLuint program, const char* name, GLfloat* v);
void set_uniform4fv(GLuint program, const char* name, GLfloat* v);

void set_uniform_mat4f(GLuint program, const char* name, GLfloat* mat);
void set_uniform_mat3f(GLuint program, const char* name, GLfloat* mat);






//textures
GLboolean load_texture2D(const char* filename, GLenum min_filter, GLenum mag_filter, GLenum wrap_mode, GLboolean flip);
GLboolean load_texture_cubemap(const char* filename[], GLenum min_filter, GLenum mag_filter, GLboolean flip);
