#version 330 core

in vec2 vTextureCoord;
in vec3 vLightWeighting;

uniform bool uUseTextures;
uniform sampler2D uSampler;

out vec4 FragColor;

void main(void) {
	vec4 fragmentColor;
	if (uUseTextures) {
		fragmentColor = texture2D(uSampler, vTextureCoord);
	} else {
		fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	FragColor = vec4(fragmentColor.rgb * vLightWeighting, fragmentColor.a);
}

