// Fragment Shader
#version 330

in vec4 varying_color;

out vec4 frag_color;
void main(void)
{
	vFragColor = varying_color;
}

