
#ifndef RSW_MATSTACK
#define RSW_MATSTACK

#include "rsw_math.h"
#include "rsw_glframe.h"

using rsw::vec3;
using rsw::vec4;
using rsw::mat4;
using rsw::mat3;


enum MATSTACK_ERROR { MATSTACK_NOERROR = 0, MATSTACK_OVERFLOW, MATSTACK_UNDERFLOW }; 

struct matrix_stack
{
	int capacity;
	mat4* stack;
	int top;

	matrix_stack(int cap = 64)
	{
		capacity = cap;
		stack = new mat4[cap];
		top = 0;
		last_error = MATSTACK_NOERROR;
	}
	
	~matrix_stack(void) { delete []stack; }

	void load_identity(void)
	{
		stack[top] = mat4();	//should I make a matrix load identity function
	}
	
	void load_mat(const mat4 m)
	{ 
		stack[top] = m; 
	}
        
    void load_mat(GLFrame& frame)
	{
        load_mat(frame.get_matrix());
	}
    
	//one of these days I should really try restrict keyword and see if it makes a significant difference
	void mult_mat(const mat4 m)
	{
		stack[top] = stack[top] * m;

	}
        
    void mult_mat(GLFrame& frame)
	{
        mult_mat(frame.get_matrix());
	}
        				
	void push_mat(void)
	{
		if (top < (capacity-1)) {
			top++;
			stack[top] = stack[top-1];
		} else {
			last_error = MAT_STACK_OVERFLOW;
		}
	}
	
	void pop(void)
	{
		if(top > 0)
			top--;
		else
			last_error = MAT_STACK_UNDERFLOW;
	}
		
	void scale(float x, float y, float z)
	{
		stack[top] = stack[top] * rsw::scale_mat4(x, y, z);
	}
		
		
	void Translate(float x, float y, float z)
	{
		stack[top] = stack[top]*rsw::TranslationMatrix44(x, y, z);		
	}
        			
	void Rotate(float angle, float x, float y, float z)
	{
		mat4 rotate;
		rotmat44f(rotate, vec3(x, y, z), angle);
		stack[top] = stack[top]*rotate;
	}
	
	
	// I've always wanted vector versions of these
	void Scalev(const vec3 vScale)
	{
		stack[top] = stack[top]*rsw::ScaleMatrix44(vScale);
	}
		
	void Translatev(const vec3 vTranslate)
	{
		stack[top] = stack[top]*rsw::TranslationMatrix44(vTranslate);	
	}
    
		
	void Rotatev(float angle, vec3 vAxis)
	{
		mat4 rotate;
		rotmat44f(rotate, vAxis, angle);
		stack[top] = stack[top]*rotate;
	}
		
	
	// I've also always wanted to be able to do this
	void push_mat(const mat4 m)
	{
	 	if(top < (capacity-1)) {
			top++;
			stack[top] = m;
		} else {
			last_error = MATSTACK_OVERFLOW;
		}
	}
		
    void push_mat(GLFrame& frame)
	{
		push_mat(frame.get_matrix());
	}
        
	// Two different ways to get the matrix
	const mat4& get_matrix(void) { return stack[top]; }	//const!
	void get_matrix(mat4 m) { m = stack[top]; }	//copy


	MAT_STACK_ERROR GetLastError(void)
	{
		MATSTACK_ERROR ret = last_error;
		last_error = MATSTACK_NOERROR;
		return ret; 
	}

};






#endif

