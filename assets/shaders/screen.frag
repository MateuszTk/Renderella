#version 430 core

layout (location = 0) out vec4 FragColor;
  
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

uniform vec3 viewPos;
uniform vec3 viewDir;

uniform vec2 nearFar;
#define NEAR nearFar.x
#define FAR nearFar.y

uniform mat4 projectionView;
uniform mat4 view;
uniform mat4 projection;

#define SPECULARITY_THRESHOLD 0.01

const float maxDistance = 8.0;
const float resolution = 0.25;
const float thickness = 0.005;

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

vec3 viewPosFromWorld(vec3 worldPos) {
	return vec3(view * vec4(worldPos, 1.0));
}

vec3 traceSSRay(vec3 startScreen, vec3 endScreen) {
	vec2 depthTextureSize = textureSize(depthTexture, 0);
	vec2 depthTextureTexelSize = 1.0 / depthTextureSize;
	startScreen.xy = (startScreen.xy * 0.5 + 0.5) * depthTextureSize;
	endScreen.xy = (endScreen.xy * 0.5 + 0.5) * depthTextureSize;

	vec2 deltaXY = vec2(endScreen - startScreen);
	vec2 deltaXYinv = 1.0 / deltaXY;
	bool useX = (abs(deltaXY.x) >= abs(deltaXY.y));
	// how many steps to make in the longest direction
	float deltaPix = (useX == true) ? abs(deltaXY.x) : abs(deltaXY.y);
	float delta = deltaPix * resolution;

	vec2 incrementPix = deltaXY / max(deltaPix, 0.0001);
	vec2 increment = deltaXY / max(delta, 0.0001);

	vec2 currentPos = startScreen.xy;
	float progress = 0.0;
	const float ssZesZ = startScreen.z * endScreen.z;
	const int maxSteps = min(int(delta), 600);
	for (int i = 0; i < maxSteps; i++) {
		currentPos += increment;		
		progress = mix((currentPos.x - startScreen.x) * deltaXYinv.x, (currentPos.y - startScreen.y) * deltaXYinv.y, useX);

		// perspective correct interpolation
		float positionZ = ssZesZ / mix(endScreen.z, startScreen.z, progress);

		vec2 uv = currentPos * depthTextureTexelSize;
		if (uv.x > 1.0 || uv.x < 0.0 || uv.y > 1.0 || uv.y < 0.0 || positionZ <= 0.0 || positionZ > 1.0 || i + 1 == maxSteps) {
			progress = 1.0;
			break;
		}
		
		float depthValue = texture(depthTexture, uv).r;
		float depthDifference = positionZ - depthValue;
		//float depthDifference = linearizeDepth(positionZ, NEAR, FAR) - linearizeDepth(depthValue, NEAR, FAR);
		if (depthDifference > 0.0 && depthDifference < thickness) {
			currentPos -= increment;

			// precise search
			const int preciseSteps = int(1.0 / resolution);
			for (int j = 0; j < preciseSteps; j++) {
				currentPos += incrementPix;
				uv = currentPos * depthTextureTexelSize;
				depthValue = texture(depthTexture, uv).r;
				progress = mix((currentPos.x - startScreen.x) * deltaXYinv.x, (currentPos.y - startScreen.y) * deltaXYinv.y, useX);
				positionZ = ssZesZ / mix(endScreen.z, startScreen.z, progress);
				depthDifference = positionZ - depthValue;
				if (depthDifference > 0.0 && depthDifference < thickness) {
					break;
				}
			}

			break;
		}
	}
	return vec3(currentPos / depthTextureSize, progress);
}

vec4 ssr(vec3 worldPixelPos){
	vec3 VSPostion = vec3(view * vec4(worldPixelPos, 1.0));
	vec3 VSNormal = normalize(vec3(view * vec4(texture(screenTexture2, TexCoords).rgb * 2.0 - 1.0, 0.0)));

	vec3 reflectionDir = normalize(reflect(normalize(VSPostion), VSNormal));

	vec4 startVS = vec4(VSPostion, 1.0);
	float dist = ((reflectionDir.z > 0.0) ? (-VSPostion.z - NEAR) : (FAR - VSPostion.z)) - 0.001;
	vec4 endVS = vec4(VSPostion + reflectionDir * dist, 1.0);

	if (endVS.z > -NEAR - 0.001) {
		return vec4(0.0);
	}

	vec4 startScreen = projection * startVS;
	startScreen.xyz /= startScreen.w;

	vec4 endScreen = projection * endVS;
	endScreen.xyz /= endScreen.w;

	startScreen.z = (startScreen.z + 1.0) / 2.0;
	endScreen.z = (endScreen.z + 1.0) / 2.0;

	vec3 traced = traceSSRay(startScreen.xyz, endScreen.xyz);
	float visibility = 1.0;
	if (traced.z >= 1.0) {
		visibility = 0.0;
	}

	//(1.0 - max(dot(-normalize(VSPostion), reflectionDir), 0.0));// * (1.0 - traced.z);
	return vec4(traced, visibility);
}

void main() {
	float depthValue = texture(depthTexture, TexCoords).r;
	if (depthValue < 1.0) {
		vec3 worldPixelPos = worldFragmentPos(FragPos, viewDir, depthValue);

		vec4 lightData = texture(screenTexture1, TexCoords);
		
		if (lightData.w >= SPECULARITY_THRESHOLD) {
			vec4 ssrUV = ssr(worldPixelPos).xywz;
			FragColor = ssrUV;
		}
		else {
			FragColor = vec4(0.0);
		}
	}
	else{
		FragColor = vec4(0.0);
	}
}
