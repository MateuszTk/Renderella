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
uniform sampler2D lightDepth;

uniform sampler2D sky;

uniform vec4 lightPos[8];
uniform vec3 lightColor[8];
uniform vec3 lightDir[8];
uniform mat4 lightSpaceMatrix[8];
uniform int usedLights;
uniform vec3 viewPos;
uniform vec3 viewDir;

#define FAR 100.0
#define NEAR 0.4

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

float directionalShadow(vec4 lightSpacePos, int lightIndex) {
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	
	float currentDepth = projCoords.z;
	float bias = 0.005;

	int kernelSize = 1;
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
				vec4 lightSpacePos = lightSpaceMatrix[i] * vec4(worldPixelPos, 1.0);
				diffuseF += lightColor[i] * (1.0 - directionalShadow(lightSpacePos, i));
			}
			else if (lightType == 0) {
				// Point light
				float distance = length(lightPos[i].xyz - worldPixelPos);
				attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * distance * distance);
			}

			light += attenuation * diffuseF;
			specularReflection += attenuation * specularF;
		}
		
		float roughness = 1.0 - specular;
		roughness *= roughness;
		vec3 reflectDir = reflect(normalize(CameraRay), normal);
		vec3 skyReflection = getSkyColor(reflectDir, roughness);

		LightColor = vec4(light, 1.0);
		float brightness = max(0, max(LightColor.x, max(LightColor.y, LightColor.z)) - 1.0);
		LightColor.xyz = LightColor.xyz / (brightness + 1);
		LightColor.w = brightness / 4.0;

		ReflectionColor = vec4(skyReflection + specularReflection, lightData.w);
	}
	else{
		LightColor = vec4(0.0);
		ReflectionColor = vec4(getSkyColor(CameraRay, 0.0), 1.0);
	}
}
