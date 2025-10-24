#version 330 core

in vec2 vTextureCoord;
in vec3 vTransformedNormal;
in vec4 vPosition;

uniform bool uUseLighting;
uniform bool uUseTextures;

uniform vec3 uAmbientColor;

uniform vec3 uPointLightingLocation;
uniform vec3 uPointLightingColor;

uniform sampler2D uSampler;

out vec4 FragColor;

void main(void)
{
	vec3 lightWeighting;
	if (!uUseLighting) {
		lightWeighting = vec3(1.0, 1.0, 1.0);
	} else {
		vec3 lightDirection = normalize(uPointLightingLocation - vPosition.xyz);

		float directionalLightWeighting = max(dot(normalize(vTransformedNormal), lightDirection), 0.0);
		lightWeighting = uAmbientColor + uPointLightingColor * directionalLightWeighting;
	}

	vec4 fragmentColor;
	if (uUseTextures) {
		fragmentColor = texture2D(uSampler, vTextureCoord);
	} else {
		fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	FragColor = vec4(fragmentColor.rgb * lightWeighting, fragmentColor.a);
}

