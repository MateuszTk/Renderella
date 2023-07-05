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
	
	float currentDepth = projCoords.z;
	float bias = 0.004;

	const int kernelSize = 1;
	vec2 texelSize = 1.0 / textureSize(lightDepth, 0);
	float shadow = 0.0;
	for (int y = -kernelSize; y <= kernelSize; y += 1) {
		for (int x = -kernelSize; x <= kernelSize; x += 1) {
			float closestDepth = texture(lightDepth, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (currentDepth - bias > closestDepth ? 1.0 : 0.0);
		}
	}
	shadow /= (2 * kernelSize + 1) * (2 * kernelSize + 1);
	return shadow;
}

void main() {
	float depthValue = texture(depthTexture, TexCoords).r;
	vec3 worldPixelPos = worldFragmentPos(FragPos, viewDir, depthValue);

	vec3 light = vec3(0.0);

	for (int i = 0; i < usedLights; i++){
		int lightType = int(lightPos[i].w);
		// Directional light
		if (lightType == 1) {
			vec4 lightSpacePos = lightSpaceMatrix[i] * vec4(worldPixelPos, 1.0);
			light += lightColor[i] * (1.0 - directionalShadow(lightSpacePos, i));
		}
	}
	
	light += vec3(0.2);

	FragColor = vec4(texture(screenTexture, TexCoords).rgb * light.rgb, 1.0);
}
