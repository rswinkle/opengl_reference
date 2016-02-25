// Gouraud lighting no textures
// Fragment Shader
// Robert Winkler
#version 330 core

in vec3 color;


out vec4 frag_color;

void main(void)
{
	frag_color = vec4(color, 1);
	
}
    
