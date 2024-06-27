#version 430 core

layout (triangles, invocations = 4) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 projectionView[4];

void main()
{
	gl_Position = projectionView[gl_InvocationID] * gl_in[0].gl_Position;
	gl_Layer = gl_InvocationID;
	EmitVertex();
	gl_Position = projectionView[gl_InvocationID] * gl_in[1].gl_Position;
	gl_Layer = gl_InvocationID;
	EmitVertex();
	gl_Position = projectionView[gl_InvocationID] * gl_in[2].gl_Position;
	gl_Layer = gl_InvocationID;
	EmitVertex();
	EndPrimitive();
}
