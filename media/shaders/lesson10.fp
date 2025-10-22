#version 330 core

in vec2 vTextureCoord;

uniform sampler2D uSampler;

out vec4 FragColor;

void main(void)
{
	FragColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t));
}

