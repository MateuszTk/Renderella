#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

uniform vec3 lightPos[8];
uniform vec3 lightColor[8];
uniform int usedLights;
uniform vec3 viewPos;

uniform vec3 ambient;
uniform float specular;
uniform float shininess;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main() {
	vec4 objectColor = texture(diffuseMap, TexCoord);
	if (objectColor.a < 0.5) {
		discard;
	}

	vec3 norm = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
	norm = normalize(TBN * norm);

	for (int i = 0; i < usedLights; i++) {
		// from light to fragment
		vec3 lightDir = normalize(lightPos[i] - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffusev = diff * lightColor[i];

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		vec3 specularv = specular * spec * lightColor[i];

		float distance = length(lightPos[i] - FragPos);
		float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * distance * distance);

		vec3 result = (diffusev + specularv) * attenuation;
		FragColor += vec4(result, 1.0f);
	}

	FragColor = vec4((FragColor.rgb + ambient) * objectColor.rgb, 1.0f);
}
