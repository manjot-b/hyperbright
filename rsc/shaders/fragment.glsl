#version 330 core

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main()
{
	//fragColor = vertexColor;
	fragColor = texture(tex, texCoord);
}
