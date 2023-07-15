#version 430 core

out vec4 FragColor;
  
in vec2 TexCoords;
in vec3 FragPos;
in vec3 CameraRay;

uniform sampler2D colorTexture;
uniform sampler2D deferredLight;
uniform sampler2D deferredReflection;
uniform sampler2D ssrTexture;
uniform sampler2D depthTexture;

void main() {
	// Deferred
	vec4 fragLight = texture(deferredLight, TexCoords);
	fragLight.rgb *= fragLight.a * 4.0 + 1.0;
	vec4 fragReflection = texture(deferredReflection, TexCoords);
	float specular = fragReflection.w;

	float depthValue = texture(depthTexture, TexCoords).r;
	if (depthValue < 1.0) {
		// SSR
		vec4 ssr = texture(ssrTexture, TexCoords);
		vec2 unpackedSSR = vec2(ssr.x + ssr.y / 255.0, ssr.z + ssr.w / 255.0);
		vec4 ssrLight = texture(deferredLight, unpackedSSR);
		ssrLight.rgb *= ssrLight.a * 4.0 + 1.0;
		vec4 ssrReflection = texture(deferredReflection, unpackedSSR);
		float ssrSpecular = ssrReflection.w;
		vec3 ssrColor = texture(colorTexture, unpackedSSR).xyz * (ssrLight.xyz + ssrReflection.xyz * ssrSpecular);

		// Mix
		vec3 color = texture(colorTexture, TexCoords).xyz;
		vec3 mixedReflection = mix(fragReflection.xyz, ssrColor, ssr.x > 0.0) * specular;
		
		FragColor = vec4(color * (fragLight.xyz + mixedReflection), 1.0);
	}
	else{
		FragColor = vec4(fragReflection.xyz, 1.0);
	}
}
