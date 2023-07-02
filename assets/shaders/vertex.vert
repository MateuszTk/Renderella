#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out mat3 TBN;

uniform mat4 transformations;
uniform mat4 model;
uniform mat3 normalMatrix;

void main() {
	

	TexCoord = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));

	vec3 T = normalize(vec3(model * vec4(normalize(aTangent), 0.0)));
	vec3 B = normalize(vec3(model * vec4(normalize(aBitangent), 0.0))); //cross(aNormal, aTangent)), 0.0)));//
	vec3 N = normalize(vec3(model * vec4(normalize(aNormal), 0.0)));
	TBN = mat3(T, B, N);

	Normal = normalMatrix * aNormal;

	gl_Position = transformations * vec4(aPos, 1.0);	
}
