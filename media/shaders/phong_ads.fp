// Simple Phong lighting

// only normal coming in
// non-local viewer
// directional light

// Fragment Shader
// Robert Winkler
#version 330




uniform vec3 material_color;
uniform vec3 light_color;

// d, s, shininess
uniform float diff_intensity;
uniform float spec_intensity;
uniform float shininess;

in vec3 eye_normal;


out vec4 fragcolor;

void main(void)
{
	// add ambient
	vec3 out_light = material_color;
	
	//non-local viewer and constant directional light
	vec3 light_dir = vec3(-1, 1, 0.5);
	vec3 eye_dir = vec3(0, 0, 1);

	vec3 s = normalize(light_dir);
	vec3 n = normalize(eye_normal);
	vec3 v = eye_dir;
	
	
	// Dot product gives us diffuse intensity
	float diff = max(0.0, dot(s, n));

	// add diffuse light
	out_light += diff * diff_intensity;

	// Specular Light
	vec3 r = reflect(-s, n);
	
	float spec = max(0.0, dot(r, v));
	if(diff > 0) {
		float shine = pow(spec, shininess);
		out_light += spec_intensity * shine;
	}
	
	fragcolor = vec4(out_light, 1);
	
}

