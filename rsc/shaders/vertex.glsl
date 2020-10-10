#version 330 core

layout (location = 0) in vec3 inPosition;
//layout (location = 1) in vec3 inColor;
layout (location = 1) in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec4 vertexColor;

void main()
{
    gl_Position = perspective * view * model * vec4(inPosition, 1.0);
	//vertexColor = vec4(inColor, 1.0);
	vertexColor = vec4(inNormal, 1.0);
}
