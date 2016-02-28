//Simple texturing
//Fragment Shader
// Robert Winkler
#version 330 core

uniform sampler2D color_map;


smooth in vec2 texcoords;

out vec4 frag_color;

void main(void)
{
    frag_color = texture(color_map, texcoords);
}

