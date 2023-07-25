#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragLight;
layout (location = 2) out vec4 FragNormal;

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D shininessMap;

void main() {
	vec4 objectColor = texture(diffuseMap, TexCoord);
	if (objectColor.a < 0.01) {
		discard;
	}

	vec3 norm = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
	norm = normalize(TBN * norm);

	FragColor = vec4(objectColor.rgb * diffuse, objectColor.a);
	vec3 specularTexColor = texture(specularMap, TexCoord).rgb;
	FragLight = vec4(texture(shininessMap, TexCoord).r * clamp(shininess / 1000.0, 0.0, 1.0), max(specularTexColor.r, max(specularTexColor.g, specularTexColor.b)) * specular.r, 0.0, objectColor.a < 0.99 ? 0.0 : 1.0);
	FragNormal = vec4(norm * 0.5 + 0.5, objectColor.a < 0.99 ? 0.0 : 1.0);
}
