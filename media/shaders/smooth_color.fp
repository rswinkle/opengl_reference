// Fragment Shader
#version 330

in vec4 vary_color;

out vec4 frag_color;

void main(void)
{
	frag_color = vary_color;
}

