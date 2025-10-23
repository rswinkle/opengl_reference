#version 330 core

in vec2 vTextureCoord;
in vec3 vLightWeighting;

uniform sampler2D uSampler;

out vec4 FragColor;

void main(void) {
	vec4 textureColor = texture2D(uSampler, vTextureCoord);
	FragColor = vec4(textureColor.rgb * vLightWeighting, textureColor.a);
}
