#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform vec3 ambient;
uniform float specular;
uniform float shininess;

uniform sampler2D texture1;

void main() {
	vec4 objectColor = texture(texture1, TexCoord);
	if (objectColor.a < 0.1) {
		discard;
	}

	vec3 norm = normalize(Normal);
	// from light to fragment
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffusev = diff * lightColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specularv = specular * spec * lightColor;

	vec3 result = (ambient + diffusev + specularv) * vec3(objectColor);
	FragColor = vec4(result, objectColor.a);
}
