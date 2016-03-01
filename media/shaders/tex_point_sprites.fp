// Point Sprites
// Fragment Shader
// Robert Winkler
#version 330 core

uniform sampler2D tex;

out vec4 fragcolor;

void main(void)
{
	vec2 p = gl_PointCoord*2.0 - vec2(1.0);

	//minkowski distance
	const float power = 8.0;
	float len = pow(pow(abs(p.x), power) + pow(abs(p.y), power), 1.0/power);
	if (len > 1.0)
		discard;
	fragcolor = texture2D(tex, gl_PointCoord);
}

