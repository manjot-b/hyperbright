#version 330 core

in vec2 texCoord;

uniform sampler2D tex;
uniform bool hasTexture;
uniform vec4 vertexColor;

out vec4 fragColor;

void main()
{
	if (hasTexture)
	{
		fragColor = texture(tex, texCoord);
	}
	else
	{
		fragColor = vertexColor;
	}
}
