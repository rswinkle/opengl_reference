#version 330 core

in vec2 vTextureCoord;
in vec3 vTransformedNormal;
in vec4 vPosition;

uniform float uShininess;

uniform bool uShowSpecular;
uniform bool uUseLighting;
uniform bool uUseTextures;

uniform vec3 uAmbientColor;

uniform vec3 uPntLight;
uniform vec3 uPntLightSpec;
uniform vec3 uPntLightDiff;

uniform sampler2D uSampler;

out vec4 FragColor;

void main(void)
{
	vec3 lightWeighting;
	if (!uUseLighting) {
		lightWeighting = vec3(1.0, 1.0, 1.0);
	} else {
		vec3 lightDirection = normalize(uPntLight - vPosition.xyz);
		vec3 normal = normalize(vTransformedNormal);

		float specularLightWeighting = 0.0;
		if (uShowSpecular) {
			vec3 eyeDirection = normalize(-vPosition.xyz);
			vec3 reflectionDirection = reflect(-lightDirection, normal);

			specularLightWeighting = pow(max(dot(reflectionDirection, eyeDirection), 0.0), uShininess);
		}

		float diffuseLightWeighting = max(dot(normal, lightDirection), 0.0);
		lightWeighting = uAmbientColor
			+ uPntLightSpec * specularLightWeighting
			+ uPntLightDiff * diffuseLightWeighting;
	}

	vec4 fragmentColor;
	if (uUseTextures) {
		fragmentColor = texture2D(uSampler, vTextureCoord);
	} else {
		fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	FragColor = vec4(fragmentColor.rgb * lightWeighting, fragmentColor.a);
}
