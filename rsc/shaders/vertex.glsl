#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec4 vertexColor;
out vec2 texCoord;
out vec3 n;
out vec3 vertexPos;

void main()
{
    gl_Position = perspective * view * model * vec4(inPosition, 1.0);
	texCoord = inTexCoord;
	n = vec3(model * vec4(inNormal,0.f));
	vertexPos = vec3(model * vec4(inPosition,0.f));
}
