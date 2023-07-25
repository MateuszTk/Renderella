#version 430 core

layout (location = 0) out vec4 FragColor;
  
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

uniform sampler2D prevFrame;
uniform	sampler2D prevSSR;
uniform sampler2D prevDepth;
uniform mat4 prevProjectionView;

uniform int frameCounter;

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

float hash1(inout float seed) {
	return fract(sin(seed += 0.1) * 43758.5453123);
}

vec2 hash2(inout float seed) {
	return fract(sin(vec2(seed += 0.1, seed += 0.1)) * vec2(43758.5453123, 22578.1459123));
}

vec3 hash3(inout float seed) {
	return fract(sin(vec3(seed += 0.1, seed += 0.1, seed += 0.1)) * vec3(43758.5453123, 22578.1459123, 19642.3490423));
}

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

	float seed = FragPos.x + FragPos.y * 3.43121412313 + fract(1.12345314312 * float(frameCounter));
	float roughness = 1.0 - texture(screenTexture1, TexCoords).r;
	roughness *= roughness;
	vec3 jitter = (hash3(seed) * 2.0 - 1.0) * roughness * 0.5;
	vec3 reflectionDir = normalize(reflect(normalize(VSPostion), VSNormal) + jitter);

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

	return vec4(traced, visibility);
}

void main() {
	float depthValue = texture(depthTexture, TexCoords).r;
	if (depthValue < 1.0) {
		vec3 worldPixelPos = worldFragmentPos(FragPos, viewDir, depthValue);

		float specular = texture(screenTexture1, TexCoords).g;
		
		if (specular >= SPECULARITY_THRESHOLD) {
			vec4 ssrUV = ssr(worldPixelPos);
			vec4 ssrColor = vec4(texture(prevFrame, ssrUV.xy).xyz, ssrUV.w);
			
			// temporal reprojection
			vec4 previousProjection = prevProjectionView * vec4(worldPixelPos, 1.0);
			previousProjection.xyz /= previousProjection.w;
			previousProjection.xyz = (previousProjection.xyz + 1.0) * 0.5;

			if (abs(texture(prevDepth, previousProjection.xy).r - previousProjection.z) < 0.01) {
				vec4 prevSSRColor = texture(prevSSR, previousProjection.xy);
				FragColor = (ssrColor * 0.1 + prevSSRColor) / 1.1;
			}
			else{
				FragColor = vec4(ssrColor.xyz, 0.0);
			}
		}
		else {
			FragColor = vec4(0.0);
		}
	}
	else{
		FragColor = vec4(0.0);
	}
}
