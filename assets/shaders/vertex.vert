#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 instanceModel;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

uniform mat4 projectionView;
uniform mat4 model;

void main() {
	mat4 actualModel;
	if (gl_InstanceID == 0) {
		actualModel = model;
	} else {
		actualModel = instanceModel;
	}

	TexCoord = aTexCoord;
	FragPos = vec3(actualModel * vec4(aPos, 1.0));

	vec3 T = normalize(vec3(actualModel * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(actualModel * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(actualModel * vec4(aNormal, 0.0)));
	TBN = mat3(T, B, N);

	gl_Position = projectionView * actualModel * vec4(aPos, 1.0);
	//gl_Position = transformations * vec4(aPos, 1.0);	
}
