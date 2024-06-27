#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 CurrentSSR;
  
in vec2 TexCoords;
in vec3 FragPos;
in vec3 CameraRay;

uniform sampler2D colorTexture;
uniform sampler2D deferredLight;
uniform sampler2D deferredReflection;
uniform sampler2D ssrTexture;
uniform sampler2D depthTexture;

void main() {
	CurrentSSR = texture(ssrTexture, TexCoords);

	// Deferred
	vec4 fragLight = texture(deferredLight, TexCoords);
	fragLight.rgb *= fragLight.a * 4.0 + 1.0;
	vec4 fragReflection = texture(deferredReflection, TexCoords);
	float specular = fragReflection.w;

	float depthValue = texture(depthTexture, TexCoords).r;
	vec4 color = texture(colorTexture, TexCoords);
	if (depthValue < 1.0) {
		// SSR		
		vec2 ssrTexelSize = 1.0 / vec2(textureSize(ssrTexture, 0));
		vec4 ssrColor = vec4(0.0);
		for (int y = -1; y <= 1; ++y) {
			for (int x = -1; x <= 1; ++x) {
				vec2 offset = vec2(x, y) * ssrTexelSize;
				vec4 ssr = texture(ssrTexture, TexCoords + offset);
				ssrColor += ssr;
			}
		}
		ssrColor /= 9.0;

		// Mix
		vec3 mixedReflection = mix(fragReflection.xyz, ssrColor.xyz, ssrColor.w) * specular;
		
		FragColor = vec4(color.xyz * (fragLight.xyz + mixedReflection), 1.0);
	}
	else{
		FragColor = vec4(mix(fragReflection.xyz, color.xyz, color.a), 1.0);
	}	
}
