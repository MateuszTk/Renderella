#version 430 core

#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 LightColor;
layout (location = 1) out vec4 ReflectionColor;
  
in vec2 TexCoords;
in vec3 FragPos;
in vec3 CameraRay;

// color: xyz, shininess: w
uniform sampler2D screenTexture0;
// specular: w
uniform sampler2D screenTexture1;
//normal: xyz
uniform sampler2D screenTexture2;
// depth
uniform sampler2D depthTexture;
// light depth
uniform sampler2DShadow lightDepthShadowSampler;
uniform sampler2D lightDepth;

uniform sampler2D sky;

uniform vec4 lightPos[8];
uniform vec3 lightColor[8];
uniform vec3 lightDir[8];
uniform mat4 lightSpaceMatrix[8];
uniform int usedLights;
uniform vec3 viewPos;
uniform vec3 viewDir;

uniform vec2 nearFar;
#define NEAR nearFar.x
#define FAR nearFar.y

const vec2 poissonDisk[16] = {
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 ),
	vec2( -0.91588581, 0.45771432 ),
	vec2( -0.81544232, -0.87912464 ),
	vec2( -0.38277543, 0.27676845 ),
	vec2( 0.97484398, 0.75648379 ),
	vec2( 0.44323325, -0.97511554 ),
	vec2( 0.53742981, -0.47373420 ),
	vec2( -0.26496911, -0.41893023 ),
	vec2( 0.79197514, 0.19090188 ),
	vec2( -0.24188840, 0.99706507 ),
	vec2( -0.81409955, 0.91437590 ),
	vec2( 0.19984126, 0.78641367 ),
	vec2( 0.14383161, -0.14100790 )
};

#define LIGHT_SIZE_UV 4.0
#define BIAS 0.00025
#define NORMAL_BIAS 0.026

float linearizeDepth(float d, float zNear, float zFar) {
    float ndc = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - ndc * (zFar - zNear));
}

vec3 worldFragmentPos(vec3 fragPos, vec3 viewDir, float depthValue) {
	float linearDepth = linearizeDepth(depthValue, NEAR, FAR);	
	float viewZDist = dot(normalize(viewDir), normalize(CameraRay));
	vec3 worldPixelPos = viewPos + (normalize(CameraRay) * (linearDepth / viewZDist));
	return worldPixelPos;
}

float penumbraSize(float zReceiver, float zBlocker) {
	return (zReceiver - zBlocker) / zBlocker;
}

vec2 PCSS_BlockerDistance(vec3 projCoords, vec2 texelSize) {
	float blockerDistance = 0.0;
	float numBlockers = 0.0;

	float searchWidth = LIGHT_SIZE_UV;

	int sampleCount = 16;
	for (int sampleN = 0; sampleN < sampleCount; sampleN += 1) {
		vec2 offset = poissonDisk[sampleN] * texelSize * searchWidth;
		float texelDepth = textureLod(lightDepth, projCoords.xy + offset, 0.0).r;
		if (texelDepth < projCoords.z + BIAS) {
			blockerDistance += texelDepth;
			numBlockers += 1.0;
		}
	}
	if (numBlockers > 0.0) {
		blockerDistance /= numBlockers;
	}

	return vec2(blockerDistance, numBlockers);
}

float PCF_filter(vec3 projCoords, vec2 texelSize, float uvRadius) {
	float sum = 0.0;
	int sampleCount = 16;
	for (int sampleN = 0; sampleN < sampleCount; sampleN++) {
		vec2 offset = uvRadius * poissonDisk[sampleN] * texelSize;
		sum += texture(lightDepthShadowSampler, vec3(projCoords.xy + offset, projCoords.z + BIAS));
	}
	return sum / float(sampleCount);
}

float directionalShadow(mat4 lightSpaceMatrix, vec3 normal, vec3 worldPixelPos) {
	worldPixelPos += normal * NORMAL_BIAS;
	vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPixelPos, 1.0);
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	
	float currentDepth = projCoords.z;

	vec2 texelSize = 1.0 / textureSize(lightDepth, 0);
	vec2 blockerDepth = PCSS_BlockerDistance(projCoords, texelSize);

	if (blockerDepth.y <= 0.0) {
		return 0.0;
	}

	float penumbra = LIGHT_SIZE_UV * penumbraSize(currentDepth, blockerDepth.x) + 0.5;

	return 1.0 - PCF_filter(projCoords, texelSize, penumbra);
}

vec3 getSkyColor(vec3 rayDir, float roughness) {
	vec3 rayNorm = normalize(rayDir);
	vec2 skyUV = vec2(
		atan(rayNorm.x, rayNorm.z) / (2.0 * PI),
		acos(rayNorm.y) / -PI
	);
	float maxLod = float(textureQueryLevels(sky) - 3);
	roughness = clamp(roughness * maxLod, 0.0, maxLod);
	return textureLod(sky, skyUV, roughness).rgb;
}

void main() {
	float depthValue = texture(depthTexture, TexCoords).r;
	if (depthValue < 1.0) {
		vec4 color = texture(screenTexture0, TexCoords);
		float shininess = color.w * 1000.0;
		vec4 lightData = texture(screenTexture1, TexCoords);
		float specular = lightData.w;
		vec3 normal = normalize((texture(screenTexture2, TexCoords).rgb - 0.5) * 2.0);
		vec3 worldPixelPos = worldFragmentPos(FragPos, viewDir, depthValue);
		vec3 cameraRayNorm = normalize(CameraRay);

		// Ambient light
		vec3 light = vec3(0.2);
		vec3 specularReflection = vec3(0.0);

		for (int i = 0; i < usedLights; i++){
			int lightType = int(lightPos[i].w);

			//Diffuse
			vec3 lightDirection = normalize((lightType == 0) ? (lightPos[i].xyz - worldPixelPos) : (-lightDir[i]));
			float diffIntensity = max(dot(lightDirection, normal), 0.0);
			vec3 diffuseF = diffIntensity * lightColor[i];

			// Specular
			vec3 lightReflectDir = reflect(-lightDirection, normal);
			float spec = pow(max(dot(-cameraRayNorm, lightReflectDir), 0.0), shininess);
			vec3 specularF = specular * spec * lightColor[i];

			float attenuation = 1.0;		
			if (lightType == 1) {
				// Directional light
				diffuseF *= 0.28;
				diffuseF += lightColor[i] * (1.0 - directionalShadow(lightSpaceMatrix[i], normal, worldPixelPos));
			}
			else if (lightType == 0) {
				// Point light
				float distance = length(lightPos[i].xyz - worldPixelPos);
				attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * distance * distance);
			}

			light += attenuation * diffuseF;
			specularReflection += attenuation * specularF;
		}
		
		float roughness = 1.0 - color.w;
		//roughness *= roughness;
		vec3 reflectDir = reflect(normalize(CameraRay), normal);
		vec3 skyReflection = getSkyColor(reflectDir, roughness);

		LightColor = vec4(light, 1.0);
		float brightness = max(0, max(LightColor.x, max(LightColor.y, LightColor.z)) - 1.0);
		LightColor.xyz = LightColor.xyz / (brightness + 1);
		LightColor.w = brightness / 4.0;

		ReflectionColor = vec4(skyReflection + specularReflection, specular);
	}
	else{
		LightColor = vec4(0.0);
		ReflectionColor = vec4(getSkyColor(CameraRay, 0.0), 1.0);
	}
}
