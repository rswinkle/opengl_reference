// Point Sprites
// Fragment Shader
// Robert Winkler
#version 330 core

uniform sampler2D tex;
uniform sampler2D dissolve_tex;

uniform float dissolve_factor;

out vec4 fragcolor;

void main(void)
{
	vec2 p = gl_PointCoord*2.0 - vec2(1.0);

	vec4 dissolve_color = texture2D(dissolve_tex, gl_PointCoord);
	if (dissolve_color.r < dissolve_factor)
		discard;

	//minkowski distance
	const float power = 8.0;
	float len = pow(pow(abs(p.x), power) + pow(abs(p.y), power), 1.0/power);
	if (len > 1.0)
		discard;
	
	fragcolor = texture2D(tex, gl_PointCoord);
}

