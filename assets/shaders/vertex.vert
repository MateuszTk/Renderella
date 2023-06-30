#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 transformations;
uniform mat4 model;
uniform mat3 normalMatrix;

void main() {
	TexCoord = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = normalMatrix * aNormal;
	gl_Position = transformations * vec4(aPos, 1.0);	
}
