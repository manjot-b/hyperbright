#version 330 core

struct Light
{
	bool isPoint;
	vec3 position;	// represents the direction if not a point light
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

#define MAX_LIGHTS 10

in vec2 texCoord;
in vec3 normal;
in vec3 vertexPos;
in vec4 instanceColor;

uniform sampler2D tex;
uniform bool hasTexture;
uniform vec3 cameraPos;
uniform bool isInstanceColor;
uniform vec4 color;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float shininess;
uniform bool isEmission;
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

out vec4 fragColor;

/*
	Returns the total light at this fragment using the Phong shading model.
*/
vec4 phong()
{
	//basic phong shading
	vec3 ambient = vec3(.1f);
	vec3 diffuse = vec3(.0f);
	vec3 specular = vec3(.0f);

	vec3 viewDir = normalize(cameraPos - vertexPos);
	vec3 unitNormal = normalize(normal);

	for (int i = 0; i < lightCount; i++)
	{
		float attenuation = 1;
		vec3 lightDir;
		if (lights[i].isPoint) {
			lightDir = normalize(lights[i].position - vertexPos);
			float dist = length(lights[i].position - vertexPos);
			attenuation = 1.f / (lights[i].constant + lights[i].linear * dist + lights[i].quadratic * dist * dist);
		} else {
			lightDir = normalize(-lights[i].position);
		}
			
		diffuse += max(dot(lightDir, unitNormal), 0.0) * diffuseCoeff * lights[i].color * attenuation;
		vec3 reflect = 2 * dot(lightDir, unitNormal) * unitNormal - lightDir;
		reflect = normalize(reflect);
		specular += pow(max(dot(reflect, viewDir), 0.0), shininess) * specularCoeff * lights[i].color * attenuation;
	}

	vec4 finalLight = vec4((ambient + diffuse + specular), 1.f);
	return finalLight;
}

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

		
		fragColor = phong() * vertexColor;
	}
	else
	{
		fragColor = color;
	}
}
