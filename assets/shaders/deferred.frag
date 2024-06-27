#version 430 core

#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 LightColor;
layout (location = 1) out vec4 ReflectionColor;
  
in vec2 TexCoords;
in vec3 FragPos;
in vec3 CameraRay;

// color: xyzw
uniform sampler2D screenTexture0;
// shininess: x, specular: y
uniform sampler2D screenTexture1;
//normal: xyz
uniform sampler2D screenTexture2;
// depth
uniform sampler2D depthTexture;
// light depth
uniform sampler2DArrayShadow lightDepthShadowSampler;
uniform sampler2DArray lightDepth;

uniform sampler2D sky;

uniform vec4 lightPos[2];
uniform vec3 lightColor[2];
uniform vec3 lightDir[2];
uniform mat4 lightSpaceMatrices[2 * 4];
uniform int usedLights;
uniform vec3 viewPos;
uniform vec3 viewDir;

uniform vec2 nearFar;
#define NEAR nearFar.x
#define FAR nearFar.y

#define LIGHT_SIZE_UV 12.0
#define BIAS 0.00025
#define NORMAL_BIAS (0.026*2.0)

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

float uvToCascadeId(vec2 uv) {
	return floor(log2(max(uv.x, uv.y) * 0.5) + 1.0);
}

vec2 PCSS_BlockerDistance(vec3 projCoords, vec2 texelSize, float cascade) {
	float blockerDistance = 0.0;
	float numBlockers = 0.0;

	float searchWidth = LIGHT_SIZE_UV;

	int sampleCount = 24;
	int numRings = 2;
	for (int ring = 0; ring < numRings; ring++){
		for (int sampleN = 0; sampleN < sampleCount; sampleN++) {
			float angle = float(sampleN) / float(sampleCount) * 2.0 * PI;
			vec2 offset = vec2(cos(angle), sin(angle)) * texelSize * searchWidth;

			//float cascade = uvToCascadeId(projCoords.xy + offset);
			vec2 uv = projCoords.xy + offset;
			//uv /= pow(2.0, cascade);
			float texelDepth = textureLod(lightDepth, vec3(uv, cascade), 0.0).r;
			if (texelDepth < projCoords.z + BIAS) {
				blockerDistance += texelDepth;
				numBlockers += 1.0;
			}
		}
		searchWidth /= 2.0;
	}
	if (numBlockers > 0.0) {
		blockerDistance /= numBlockers;
	}

	return vec2(blockerDistance, numBlockers);
}

float PCF_filter(vec3 projCoords, vec2 texelSize, float uvRadius, float cascade) {
	float sum = 0.0;
	int sampleCount = 16;
	int numRings = 2;
	for (int ring = 0; ring < numRings; ring++){
		for (int sampleN = 0; sampleN < sampleCount; sampleN++) {
			float angle = float(sampleN) / float(sampleCount) * 2.0 * PI;
			vec2 offset = uvRadius * vec2(cos(angle), sin(angle)) * texelSize;

			//float cascade = uvToCascadeId(projCoords.xy + offset);
			vec2 uv = projCoords.xy + offset;
			//uv /= pow(2.0, cascade);
			sum += texture(lightDepthShadowSampler, vec4(uv, cascade, projCoords.z + BIAS));
		}
		uvRadius /= 2.0;
	}
	return sum / float(sampleCount * numRings);
}


float directionalShadow(int lightSpaceMatrixIndex, vec3 normal, vec3 worldPixelPos) {
	worldPixelPos += normal * NORMAL_BIAS;
	
	int cascade = 0;

	mat4 lightSpaceMatrix = lightSpaceMatrices[4 * lightSpaceMatrixIndex + cascade];
	vec3 projCoords;
	const float margin = 0.01;

	while (cascade < 4) {
		vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPixelPos, 1.0);
		projCoords = lightSpacePos.xyz / lightSpacePos.w;
		projCoords = projCoords * 0.5 + 0.5;

		if (projCoords.x >= margin && projCoords.x <= 1.0 - margin && projCoords.y >= margin && projCoords.y <= 1.0 - margin) {
			break;
		}
		cascade++;
		lightSpaceMatrix = lightSpaceMatrices[4 * lightSpaceMatrixIndex + cascade];
	}
	
	float currentDepth = projCoords.z;

	//uvToCascadeId(projCoords.xy);

	vec2 texelSize = 1.0 / textureSize(lightDepth, 0).xy;
	vec2 blockerDepth = PCSS_BlockerDistance(projCoords, texelSize, cascade);

	if (blockerDepth.y <= 0.0) {
		return 0.0;
	}

	float penumbra = LIGHT_SIZE_UV * penumbraSize(currentDepth, blockerDepth.x) + 0.5;

	return 1.0 - PCF_filter(projCoords, texelSize, penumbra, cascade);
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
		vec4 lightData = texture(screenTexture1, TexCoords);
		float shininess = lightData.x * 1000.0;
		float specular = lightData.y;
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
				diffuseF += lightColor[i] * (1.0 - directionalShadow(i, normal, worldPixelPos));
			}
			else if (lightType == 0) {
				// Point light
				float distance = length(lightPos[i].xyz - worldPixelPos);
				attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * distance * distance);
			}

			light += attenuation * diffuseF;
			specularReflection += attenuation * specularF;
		}
		
		float roughness = 1.0 - shininess / 1000.0;
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
