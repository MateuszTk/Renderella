#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 transformations;

void main() {
	gl_Position = transformations * vec4(aPos, 1.0);	
}
