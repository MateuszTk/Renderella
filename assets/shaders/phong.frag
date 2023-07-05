#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragLight;

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

uniform vec4 lightPos[8];
uniform vec3 lightColor[8];
uniform vec3 lightDir[8];
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

	vec3 light = vec3(0.0);
	for (int i = 0; i < usedLights; i++) {
		int lightType = int(lightPos[i].w);
		
		vec3 direction;
		if(lightType == 0) {
			// from light to fragment
			direction = normalize(lightPos[i].xyz - FragPos);
		} 
		else {
			direction = normalize(-lightDir[i]);
		}
		float diff = max(dot(norm, direction), 0.0);
		vec3 diffusev = diff * lightColor[i];
		if(lightType == 1){
			//more light from sun is added later in deffered pass
			diffusev *= 0.28;
		}

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-direction, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		vec3 specularv = specular * spec * lightColor[i];

		float attenuation = 1.0;
		if (lightType == 0) {
			float distance = length(lightPos[i].xyz - FragPos);
			attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * distance * distance);
		}

		light += (diffusev + specularv) * attenuation;
	}

	FragColor = vec4(objectColor.rgb, 1.0f);
	FragLight = vec4(light, 1.0f);
}
