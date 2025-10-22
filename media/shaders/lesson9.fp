#version 330 core

in vec2 vTextureCoord;

uniform sampler2D uSampler;
uniform vec3 uColor;

out vec4 FragColor;

void main(void)
{
	vec4 textureColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t));
	FragColor = textureColor * vec4(uColor, 1.0);
}

