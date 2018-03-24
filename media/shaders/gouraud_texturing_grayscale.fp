// Gouraud lighting no textures
// Fragment Shader
// Robert Winkler
#version 330 core

uniform sampler2D color_map;


in vec3 light_intensity;
in vec2 texcoords


out vec4 frag_color;

void main(void)
{
	vec4 tex_color = texture(color_map, texcoords);
	frag_color = vec4(tex_color.rgb * light_intensity, 1);
}


