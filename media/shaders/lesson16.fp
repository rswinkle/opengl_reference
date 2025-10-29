#version 330 core

in vec2 vTextureCoord;
in vec3 vTransformedNormal;
in vec4 vPosition;

uniform vec3 uMatAmbient;
uniform vec3 uMatDiff;
uniform vec3 uMatSpec;
uniform float uMatShininess;
uniform vec3 uMatEmissive;

uniform bool uShowSpec;
uniform bool uUseTextures;

uniform vec3 uAmbLight;

uniform vec3 uPntLight;
uniform vec3 uPntLightDiff;
uniform vec3 uPntLightSpec;

uniform sampler2D uSampler;

out vec4 FragColor;

void main(void)
{
	vec3 ambientLightWeighting = uAmbLight;

	vec3 lightDirection = normalize(uPntLight - vPosition.xyz);
	vec3 normal = normalize(vTransformedNormal);

	vec3 specularLightWeighting = vec3(0.0);
	if (uShowSpec) {
		vec3 eyeDirection = normalize(-vPosition.xyz);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		float specularLightBrightness = pow(max(dot(reflectionDirection, eyeDirection), 0.0), uMatShininess);
		specularLightWeighting = uPntLightSpec * specularLightBrightness;
	}

	float diffuseLightBrightness = max(dot(normal, lightDirection), 0.0);
	vec3 diffuseLightWeighting = uPntLightDiff * diffuseLightBrightness;

	vec3 materialAmbientColor = uMatAmbient;
	vec3 materialDiffuseColor = uMatDiff;
	vec3 materialSpecularColor = uMatSpec;
	vec3 materialEmissiveColor = uMatEmissive;
	float alpha = 1.0;
	if (uUseTextures) {
		vec4 textureColor = texture2D(uSampler, vTextureCoord);
		materialAmbientColor = materialAmbientColor * textureColor.rgb;
		materialDiffuseColor = materialDiffuseColor * textureColor.rgb;
		materialEmissiveColor = materialEmissiveColor * textureColor.rgb;
		alpha = textureColor.a;
	}
	FragColor = vec4(
		materialAmbientColor * ambientLightWeighting
		+ materialDiffuseColor * diffuseLightWeighting
		+ materialSpecularColor * specularLightWeighting
		+ materialEmissiveColor,
		alpha
	);
}
