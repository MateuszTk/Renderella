#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 instanceModel;

uniform mat4 model;

#define USE_LAYERED_SHADOWS

#ifndef USE_LAYERED_SHADOWS
uniform mat4 projectionView[4];
uniform float layer;
#endif

void main() {
#ifdef USE_LAYERED_SHADOWS
	if (gl_InstanceID == 0) {
		gl_Position = model * vec4(aPos, 1.0);
	} else {
		gl_Position = instanceModel * vec4(aPos, 1.0);
	}	
#else
	int layerIndex = int(layer);
	if (gl_InstanceID == 0) {
		gl_Position = projectionView[layerIndex] * model * vec4(aPos, 1.0);
	} else {
		gl_Position = projectionView[layerIndex] * instanceModel * vec4(aPos, 1.0);
	}
#endif
}
