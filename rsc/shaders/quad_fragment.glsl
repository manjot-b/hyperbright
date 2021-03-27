#version 330 core

in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main()
{
	//float depth = texture(tex, texCoord).r;
	//fragColor = vec4(vec3(depth), 1.f);
	fragColor = texture(tex, texCoord);
}