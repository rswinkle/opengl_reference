// Simple Color 
// Fragment Shader
// Robert Winkler
#version 330 core

uniform vec4 color;

out vec4 fragcolor;

void main(void)
{
	fragcolor = color;
}

