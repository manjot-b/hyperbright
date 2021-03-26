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
uniform mat4 lightSpace;

out vec2 texCoord;
out vec3 normal;
out vec3 vertexPos;
out vec4 instanceColor;
out vec4 lightSpacePos;

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
		worldPos = model * vec4(inPosition, 1.0f);

		// Handles non-uniform scaling. If this makes the app too slow then compute
		// the normal matrix on the CPU only for models that are non-uniformly scaled.
		mat4 normalMat = transpose(inverse(model));
		normal = vec3(normalMat * vec4(inNormal, 0.f));
	}
	
    gl_Position = perspective * view * worldPos;
	texCoord = inTexCoord;

	vertexPos = worldPos.xyz;
	lightSpacePos = lightSpace * vec4(vertexPos, 1.f);

	instanceColor = inInstanceColor;
}
