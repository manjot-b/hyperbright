#version 330 core

in vec2 texCoord;
in vec3 n;
in vec3 vertexPos;
in vec4 vertexClr;

uniform sampler2D tex;
uniform bool hasTexture;
uniform vec3 light;
uniform vec3 pointOfView;
uniform float d;

out vec4 fragColor;

void main()
{
	//basic phong shading
	vec3 lightDir = normalize(light - vertexPos);
	vec3 viewDir = normalize(pointOfView);
	vec3 normal = normalize(n);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 reflect = 2 * dot(lightDir, normal) * normal - lightDir;
	reflect = normalize(reflect);
	float spec = pow(max(dot(reflect, viewDir), 0.0), 32) * 0.5;

	if (hasTexture)
	{
		fragColor = (1 / (d * d)) * (vec4(diff * texture(tex, texCoord)) + vec4(spec * texture(tex, texCoord))) + vec4(0.1 * texture(tex, texCoord));
	}
	else
	{
		fragColor = (1 / (d * d)) * (vec4(diff * vertexClr) + vec4(spec * vertexClr)) + vec4(0.1 * vertexClr);
	}
}
