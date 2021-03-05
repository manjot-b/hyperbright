#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in mat4 instanceModel;	// mat4 takes up postions of 4 vec4s.
layout (location = 7) in vec4 inInstanceColor;

uniform bool isInstance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec2 texCoord;
out vec3 normal;
out vec3 vertexPos;
out vec4 instanceColor;

void main()
{
	vec4 worldPos;
	if (isInstance)
	{
		worldPos = (instanceModel * vec4(inPosition, 1.0f));
		normal = vec3(instanceModel * vec4(inNormal, 0.f));
	}
	else
	{
		worldPos = model* vec4(inPosition, 1.0f);
		normal = normalize(vec3(model * vec4(inNormal, 0.f)));
	}
	
    gl_Position = perspective * view * worldPos;
	texCoord = inTexCoord;

	vertexPos = worldPos.xyz;
	instanceColor = inInstanceColor;
}
