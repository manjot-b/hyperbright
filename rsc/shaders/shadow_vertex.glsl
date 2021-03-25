#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 3) in mat4 instanceModel;	// mat4 takes up postions of 4 vec4s.

uniform bool isInstance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	if (isInstance) {
		gl_Position = projection * view * instanceModel * vec4(inPosition, 1.f);
	}
	else {
		gl_Position = projection * view * model * vec4(inPosition, 1.f);
	}
}