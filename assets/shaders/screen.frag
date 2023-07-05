#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in vec3 FragPos;
in vec3 CameraRay;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform sampler2D lightDepth;

uniform vec4 lightPos[8];
uniform vec3 lightColor[8];
uniform vec3 lightDir[8];
uniform mat4 lightSpaceMatrix[8];
uniform int usedLights;
uniform vec3 viewPos;
uniform vec3 viewDir;

float linearizeDepth(float d, float zNear, float zFar) {
    float ndc = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - ndc * (zFar - zNear));
}

vec3 worldFragmentPos(vec3 fragPos, vec3 viewDir, float depthValue) {
	float linearDepth = linearizeDepth(depthValue, 0.1, 400);	
	float viewZDist = dot(normalize(viewDir), normalize(CameraRay));
	vec3 worldPixelPos = viewPos + (normalize(CameraRay) * (linearDepth / viewZDist));
	return worldPixelPos;
}

float directionalShadow(vec4 lightSpacePos, int lightIndex) {
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(lightDepth, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = 0.004;
	float shadow = (currentDepth - bias > closestDepth ? 1.0 : 0.0);
	return shadow;
}

void main() {
	float depthValue = texture(depthTexture, TexCoords).r;
	vec3 worldPixelPos = worldFragmentPos(FragPos, viewDir, depthValue);

	vec4 light = vec4(0.2);

	for (int i = 0; i < usedLights; i++){
		int lightType = int(lightPos[i].w);
		// Directional light
		if (lightType == 1) {
			vec4 lightSpacePos = lightSpaceMatrix[i] * vec4(worldPixelPos, 1.0);
			light += vec4(lightColor[i] * (1.0 - directionalShadow(lightSpacePos, i)), 1.0);
		}
	}
	
	FragColor = vec4(texture(screenTexture, TexCoords).rgb * light.rgb, 1.0);
}
