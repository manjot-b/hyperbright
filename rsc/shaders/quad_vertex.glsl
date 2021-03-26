#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;		// unused
layout (location = 2) in vec2 inTexCoord;	// at location 2 to keep VertexArray class simple

uniform mat4 model;

out vec3 vertexPos;
out vec2 texCoord;

void main()
{
	gl_Position = model * vec4(inPosition, 1.f);
	texCoord = inTexCoord;
}