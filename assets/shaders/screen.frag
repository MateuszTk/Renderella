#version 430 core

#define PI 3.1415926535897932384626433832795

out vec4 FragColor;
  
in vec2 TexCoords;
in vec3 FragPos;
in vec3 CameraRay;

// color: xyz, shininess: w
uniform sampler2D screenTexture0;
// light: xyz, specular: w
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

uniform mat4 projectionView;
uniform mat4 view;
uniform mat4 projection;

#define FAR 20.0
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

/*
// alternative method
vec3 worldFragmentPos(vec3 fragPos, vec3 viewDir, float depthValue) {	
	vec4 ndc = vec4(fragPos.xy, 2.0 * depthValue - 1.0, 1.0);
	vec4 inversed = inverse(projection) * ndc;

	inversed /= inversed.w;

	vec4 worldSpacePos = inverse(view) * inversed;
	return worldSpacePos.xyz;
}
*/

vec3 viewPosFromWorld(vec3 worldPos) {
	return vec3(view * vec4(worldPos, 1.0));
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

/*vec4 traceSSRay(vec3 rayDir) {
	vec3 rayNorm = normalize(rayDir);
	vec3 startPos = vec3(FragPos.xy, linearizeDepth(texture(depthTexture, FragPos.xy * 0.5 + 0.5).r, NEAR, FAR));
	vec3 rayPos = startPos;
	float rayLength = 0.0;
	float search = 0.0;
	const int maxSteps = 200;
	for (int i = 0; i < maxSteps; i++) {
		search += 0.005;
		rayPos = mix(startPos, startPos + rayNorm * 20.0, search);
		
		if (rayPos.x > 1.0 || rayPos.x < -1.0 || rayPos.y > 1.0 || rayPos.y < -1.0 || rayPos.z > FAR|| rayPos.z < 0.0) {
			//search = 1.0;
			break;
		}
		float depthValue = linearizeDepth(texture(depthTexture, rayPos.xy * 0.5 + 0.5).r, NEAR, FAR);
		float depth = rayPos.z;//(startPos.z * (startPos.z + rayNorm.z * 15.0)) / mix((startPos.z + rayNorm.z * 15.0), startPos.z, search);
		if (depth > depthValue && distance(rayPos.xy, FragPos.xy) > 0.01) {
			break;
		}
	}
	return vec4(rayPos * 0.5 + 0.5, search);
}*/

const float maxDistance = 8.0;
const float resolution = 0.5;
const float thickness = 0.0;

vec3 traceSSRay(vec3 startScreen, vec3 endScreen) {
	vec2 depthTextureSize = textureSize(depthTexture, 0);
	startScreen.xy = (startScreen.xy * 0.5 + 0.5) * depthTextureSize;
	endScreen.xy = (endScreen.xy * 0.5 + 0.5) * depthTextureSize;

	vec2 deltaXY = vec2(endScreen - startScreen);
	bool useX = (abs(deltaXY.x) >= abs(deltaXY.y));
	// how many steps to make in the longest direction
	float delta = ((useX == true) ? abs(deltaXY.x) : abs(deltaXY.y)) * resolution;
	delta = min(delta, 800.0);

	vec2 increment = deltaXY / max(delta, 0.001);

	//return vec3(texture(depthTexture, TexCoords).r);
	//return vec3(linearizeDepth(texture(depthTexture, TexCoords).r, NEAR, FAR));
	//return vec3(startScreen.z);

	vec2 currentPos = startScreen.xy;
	float progress = 0.0;
	for (int i = 0; i < int(delta); i++) {
		currentPos += increment;
		vec2 uv = currentPos / depthTextureSize;

		float depthValue = texture(depthTexture, uv).r;
		
		progress = mix((currentPos.x - startScreen.x) / deltaXY.x, (currentPos.y - startScreen.y) / deltaXY.y, useX);

		// perspective correct interpolation
		float positionZ = (startScreen.z * endScreen.z) / mix(endScreen.z, startScreen.z, progress);

		if (uv.x > 1.0 || uv.x < 0.0 || uv.y > 1.0 || uv.y < 0.0 || positionZ < 0.0) {
			progress = 1.0;
			break;
		}
		
		if (positionZ - depthValue > thickness) {
			break;
		}
	}

	return vec3(currentPos / depthTextureSize, progress);
}

vec4 ssr(vec3 worldPixelPos){
	vec3 VSPostion = vec3(view * vec4(worldPixelPos, 1.0));
	vec3 VSNormal = normalize(vec3(view * vec4(texture(screenTexture2, TexCoords).rgb * 2.0 - 1.0, 0.0)));

	vec3 reflectionDir = normalize(reflect(normalize(VSPostion), VSNormal)) * vec3(1.0,1.0,1.0);

	vec4 startVS = vec4(VSPostion, 1.0);
	//vec4 endVS = vec4(VSPostion + vec3(0.0, 8.0, 0.0), 1.0);
	vec4 endVS = vec4(VSPostion + reflectionDir * maxDistance, 1.0);

	vec4 startScreen = projection * startVS;
	startScreen.xyz /= startScreen.w;

	vec4 endScreen = projection * endVS;
	endScreen.xyz /= endScreen.w;

	startScreen.z = (startScreen.z + 1.0) / 2.0;
	endScreen.z = (endScreen.z + 1.0) / 2.0;

	//return vec4(abs(texture(depthTexture, TexCoords).r - startScreen.z) * 100.0);
	/*if( TexCoords.x < 0.5)
		return vec4((startScreen.z + 1.0) / 2.0);
	else{
		return vec4(texture(depthTexture, TexCoords).r);
	}*/

	vec3 traced = traceSSRay(startScreen.xyz, endScreen.xyz);
	vec4 color = vec4(0.0);

	if (traced.z < 1.0) {
		float visibility = 1.0 - max(dot(-normalize(VSPostion), reflectionDir), 0.0);

		color = vec4(texture(screenTexture0, traced.xy).rgb, visibility);
	}

	return color;
}

void main() {
	float depthValue = texture(depthTexture, TexCoords).r;
	if (depthValue < 1.0) {
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
	
		// Ambient light
		light += vec3(0.2);

		vec4 color = texture(screenTexture0, TexCoords);

		vec4 lightData = texture(screenTexture1, TexCoords);
		vec3 normal = (texture(screenTexture2, TexCoords).rgb - 0.5) * 2.0;
		vec3 reflectDir = reflect(normalize(CameraRay), normal);
		float roughness = 1.0 - color.a;
		roughness *= roughness;
		vec3 skyReflection = getSkyColor(reflectDir, roughness);

		FragColor = vec4(color.rgb * (light + lightData.xyz + skyReflection * lightData.w), 1.0);
		
		if (lightData.w > 0.6) {
			vec4 ssrColor = ssr(worldPixelPos);
			FragColor = vec4(mix(FragColor.xyz, ssrColor.xyz, ssrColor.a), 1.0);
		}
	}
	else{
		FragColor = vec4(getSkyColor(CameraRay, 0.0), 1.0);
	}
}
