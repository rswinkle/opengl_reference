// Simple Color 
// Fragment Shader
// Robert Winkler
#version 330 core

uniform vec4 color;

out vec4 vFragColor;

void main(void)
{
	vFragColor = v_color;
}

