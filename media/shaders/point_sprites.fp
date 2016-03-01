// Point Sprites
// Fragment Shader
// Robert Winkler
#version 330 core


out vec4 fragcolor;

void main(void)
{
	vec2 p = gl_PointCoord*2.0 - vec2(1.0);
	float len = dot(p, p);
	if (len > 1.0)
		discard;

	fragcolor = vec4(mod(len*3, 1), 0, 0, 1);
}

