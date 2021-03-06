#version 330 core

in vec2 texCoord;
in vec3 normal;
in vec3 vertexPos;
in vec4 instanceColor;

uniform sampler2D tex;
uniform bool hasTexture;
uniform vec3 light;
uniform vec3 pointOfView;
uniform float d;
uniform bool isInstanceColor;
uniform vec4 color;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float shininess;
uniform bool isEmission;

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
		vec3 lightDir = normalize(light - vertexPos);
		vec3 viewDir = normalize(pointOfView - vertexPos);
		float diff = max(dot(lightDir, normal), 0.0) * diffuseCoeff;
		vec3 reflect = 2 * dot(lightDir, normal) * normal - lightDir;
		reflect = normalize(reflect);
		float spec = pow(max(dot(reflect, viewDir), 0.0), shininess) * specularCoeff;
		float ambient = 0.1f;

		fragColor = (1 / (d * d)) * (ambient + diff + spec) * vertexColor;
	}
	else
	{
		fragColor = color;
	}
}
