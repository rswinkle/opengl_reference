// Simple Phong lighting

// only normal coming in
// non-local viewer
// directional light

// Fragment Shader
// Robert Winkler
#version 330

out vec4 FragColor;


// a, d, s, shininess
uniform vec4 material;

in vec3 eye_normal;

void main(void)
{
	// add ambient
	vec3 out_light = vec3(material.x);
	
	//non-local viewer and constant directional light
	vec3 light_dir = vec3(-1, 1, 0.5);
	vec3 eye_dir = vec3(0, 0, 1);

	vec3 s = normalize(light_dir);
	vec3 n = normalize(eye_normal);
	vec3 v = eye_dir;
	
	
	// Dot product gives us diffuse intensity
	float diff = max(0.0, dot(s, n));

	// add diffuse light
	out_light += diff * material.y;

	// Specular Light
	vec3 r = reflect(-s, n);
	
	float spec = max(0.0, dot(r, v));
	if(diff > 0) {
		float fSpec = pow(spec, material.w);
		out_light += material.z * fSpec;
	}
	
	FragColor = vec4(out_light, 1);
	
}

