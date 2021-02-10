#version 330 core

in vec2 texCoord;
in vec3 n;
in vec3 vertexPos;

uniform sampler2D tex;
uniform bool hasTexture;
uniform vec4 vertexColor;
uniform vec3 light;
uniform vec3 pointOfView;
uniform float d;

out vec4 fragColor;

//basic phong shading
vec3 lightDir = normalize(light - vertexPos);
vec3 viewDir = normalize(pointOfView - vertexPos);
vec3 normal = normalize(n);
float diff = max(dot(lightDir, normal), 0.0);
vec3 reflect = 2 * dot(lightDir, normal) * normal - lightDir;
float spec = pow(max(dot(reflect, viewDir), 0.0), 5);

void main()
{
	if (hasTexture)
	{
		fragColor = (1 / (d * d)) * (vec4(diff * texture(tex, texCoord)) + vec4(spec * texture(tex, texCoord))) + vec4(0.1 * texture(tex, texCoord));
	}
	else
	{
		fragColor = (1 / (d * d)) * (vec4(diff * vertexColor) + vec4(spec * vertexColor)) + vec4(0.1 * vertexColor);
	}
}
