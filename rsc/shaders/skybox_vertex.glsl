#version 330 core

layout (location = 0) in vec3 inPosition;

out vec3 texCoords;

uniform mat4 perspective;
uniform mat4 view;
uniform mat4 model;

void main()
{
    // A cubemap's direction vector is also its position vector if centered at origin.
    // Cubemaps in OpenGL also follow the left-hand coordinate system, so flip the z-axis.
    texCoords = vec3(inPosition.xy, -inPosition.z);
    gl_Position = perspective * view * model * vec4(inPosition, 1.0);
}  