#pragma once
#ifndef GLM_GLFRAME_H
#define GLM_GLFRAME_H

#include <glm/glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

struct GLFrame
{
	vec3 origin;
	vec3 forward;
	vec3 up;

	GLFrame(bool camera=false, vec3 orig = vec3(0));

	vec3 get_z() { return forward; }
	vec3 get_y() { return up; }
	vec3 get_x() { return glm::cross(up, forward); }

	void translate_world(float x, float y, float z)
		{ origin.x += x; origin.y += y; origin.z += z; }

	void translate_local(float x, float y, float z)
		{ move_forward(z); move_up(y); move_right(x); }

	void move_forward(float delta) { origin += forward * delta; }
	void move_up(float delta) { origin += up * delta; }

	void move_right(float delta)
	{
		vec3 cross = glm::cross(up, forward);
		origin += cross * delta;
	}

	mat4 get_matrix(bool rotation_only = false);
	mat4 get_camera_matrix(bool rotation_only = false);
	

	void rotate_local_y(float angle);
	void rotate_local_z(float angle);
	void rotate_local_x(float angle);

	void normalize(bool keep_forward);

	void rotate_world(float angle, float x, float y, float z);
	void rotate_local(float angle, float x, float y, float z);

	vec3 local_to_world(const vec3 local, bool rot_only = false);


};

#endif


