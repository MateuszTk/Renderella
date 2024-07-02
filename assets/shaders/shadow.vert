#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 instanceModel;

uniform mat4 model;

void main() {
	if (gl_InstanceID == 0) {
		gl_Position = model * vec4(aPos, 1.0);
	} else {
		gl_Position = instanceModel * vec4(aPos, 1.0);
	}	
}
