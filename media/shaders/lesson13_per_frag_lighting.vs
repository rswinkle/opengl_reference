#version 330 core

//w is filled with 1 automatically for vertex
//since I only passed x,y,z
layout (location = 0) in vec4 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec2 aTextureCoord;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat3 uNMatrix;


out vec2 vTextureCoord;
out vec3 vTransformedNormal;
out vec4 vPosition;

void main(void)
{
	vPosition = uMVMatrix * vec4(aVertexPosition, 1.0);
	gl_Position = uPMatrix * vPosition;
	vTextureCoord = aTextureCoord;
	vTransformedNormal = uNMatrix * aVertexNormal;
}
