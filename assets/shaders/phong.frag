#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos[8];
uniform vec3 lightColor[8];
uniform int usedLights;
uniform vec3 viewPos;

uniform vec3 ambient;
uniform float specular;
uniform float shininess;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

vec3 rotateWithSurface(vec3 normal, vec3 point)
{
    vec3 up = vec3(0.0, 1.0, 0.0); // Default rotation axis
    
    // Check if the normal is parallel to the up vector
    if (dot(normal, up) > 0.999)
    {
        return point; // No rotation needed
    }
    
    vec3 rotationAxis = normalize(cross(up, normal));
    float rotationAngle = acos(dot(up, normal));
    
    vec3 rotatedVector = point * cos(rotationAngle) + cross(rotationAxis, point) * sin(rotationAngle) + rotationAxis * dot(rotationAxis, point) * (1.0 - cos(rotationAngle));
    
    return rotatedVector;
}



void main() {
	vec4 objectColor = texture(diffuseMap, TexCoord);
	if (objectColor.a < 0.1) {
		discard;
	}

	vec3 normalColor = normalize(texture(normalMap, TexCoord).xzy * vec3(2.0) - vec3(1.0)) * vec3(-1.0, 1.0, -1.0);

	//TODO: this way of calculationg the normals is not correct
	vec3 norm = normalize(Normal);
	norm = normalize(rotateWithSurface(norm, normalColor.xyz));

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
