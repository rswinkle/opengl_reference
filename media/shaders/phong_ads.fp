// Simple Phong lighting

// only normal coming in
// non-local viewer
// directional light

// Fragment Shader
// Robert Winkler
#version 330




uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 spec_color;
uniform float shininess;

uniform vec3 light_direction;

in vec3 eye_normal;


out vec4 fragcolor;

void main(void)
{
	// add ambient
	vec3 out_light = ambient_color;
	
	//non-local viewer
	vec3 eye_dir = vec3(0, 0, 1);

	vec3 s = normalize(light_direction);
	vec3 n = normalize(eye_normal);
	vec3 v = eye_dir;
	
	// Dot product gives us diffuse intensity
	float lambertian = max(0.0, dot(s, n));
	if (lambertian > 0) {

		// add diffuse light
		out_light += lambertian * diffuse_color;

		// Specular Light
		vec3 r = reflect(-s, n);
		
		float spec = max(0.0, dot(r, v));
		float shine = pow(spec, shininess);
		out_light += spec_color * shine;
	}
	
	fragcolor = vec4(out_light, 1);
	
}

