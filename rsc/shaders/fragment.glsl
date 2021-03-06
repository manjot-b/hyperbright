#version 330 core

struct Light
{
	bool isPoint;
	vec3 position;	// represents the direction if not a point light
	vec3 color;
};

#define MAX_LIGHTS 10

in vec2 texCoord;
in vec3 normal;
in vec3 vertexPos;
in vec4 instanceColor;

uniform sampler2D tex;
uniform bool hasTexture;
uniform vec3 pointOfView;
uniform float d;
uniform bool isInstanceColor;
uniform vec4 color;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float shininess;
uniform bool isEmission;
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

out vec4 fragColor;

void main()
{
	if (!isEmission)
	{
		vec4 vertexColor;
		if (isInstanceColor)
			vertexColor = instanceColor;
		else if (hasTexture)
			vertexColor = texture(tex, texCoord);
		else
			vertexColor = color;

		//basic phong shading
		float ambient = 0.1f;
		float diffuse = 0;
		float specular = 0;

		vec3 viewDir = normalize(pointOfView - vertexPos);
		for (int i = 0; i < lightCount; i++)
		{
			vec3 lightDir;
			if (lights[i].isPoint) {
				lightDir = normalize(lights[i].position - vertexPos);
			} else {
				lightDir = normalize(-lights[i].position);
			}
			
			diffuse += max(dot(lightDir, normal), 0.0) * diffuseCoeff;
			vec3 reflect = 2 * dot(lightDir, normal) * normal - lightDir;
			reflect = normalize(reflect);
			specular += pow(max(dot(reflect, viewDir), 0.0), shininess) * specularCoeff;
		}

		fragColor = (1 / (d * d)) * (ambient + diffuse + specular) * vertexColor;
	}
	else
	{
		fragColor = color;
	}
}
