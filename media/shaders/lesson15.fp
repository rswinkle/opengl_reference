#version 330 core

in vec2 vTextureCoord;
in vec3 vTransformedNormal;
in vec4 vPosition;


uniform bool uUseColorMap;
uniform bool uUseSpecularMap;
uniform bool uUseLighting;

uniform vec3 uAmbientColor;

uniform vec3 uPntLight;
uniform vec3 uPntLightSpec;
uniform vec3 uPntLightDiff;

uniform sampler2D uColorMapSampler;
uniform sampler2D uSpecularMapSampler;

out vec4 FragColor;

void main(void)
{
	vec3 lightWeighting;
	if (!uUseLighting) {
		lightWeighting = vec3(1.0);
	} else {
		vec3 lightDirection = normalize(uPntLight - vPosition.xyz);
		vec3 normal = normalize(vTransformedNormal);

		float specularLightWeighting = 0.0;
		float shininess = 32.0;
		if (uUseSpecularMap) {
			shininess = texture2D(uSpecularMapSampler, vTextureCoord).r * 255.0;
		}
		if (shininess < 255.0) {
			vec3 eyeDirection = normalize(-vPosition.xyz);
			vec3 reflectionDirection = reflect(-lightDirection, normal);

			specularLightWeighting = pow(max(dot(reflectionDirection, eyeDirection), 0.0), shininess);
		}

		float diffuseLightWeighting = max(dot(normal, lightDirection), 0.0);
		lightWeighting = uAmbientColor
			+ uPntLightSpec * specularLightWeighting
			+ uPntLightDiff * diffuseLightWeighting;
	}

	vec4 fragmentColor;
	if (uUseColorMap) {
		fragmentColor = texture2D(uColorMapSampler, vTextureCoord);
	} else {
		fragmentColor = vec4(1.0);
	}
	FragColor = vec4(fragmentColor.rgb * lightWeighting, fragmentColor.a);
}
