#version 330 core

#define PI 3.1415926535
#define E 2.7182818284
#define EPSILON 1e-6

#define PHONG 0
#define COOK_TORRANCE 1

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
in vec4 lightSpacePos;
in vec4 instanceColor;

uniform sampler2D tex;
uniform sampler2D shadowMap;
uniform bool hasTexture;
uniform bool isInstanceColor;
uniform vec4 color;

uniform int shadingModel;	// Either PHONG or COOK_TORRANCE
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float shininess;	// used in phong

uniform float roughness;	// used in cook-torrance
uniform vec3 fresnel;
uniform bool useBeckmann;
uniform bool useGGX;

uniform vec3 cameraPos;
uniform bool isEmission;
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

out vec4 fragColor;

/*
	Get the light direction depending on the type of light and the attenutation
	stored in the last component of the vec4.
*/
vec4 lightDirection(int idx)
{
	float attenuation = 1;
	vec3 lightDir;
	if (lights[idx].isPoint) {
		lightDir = normalize(lights[idx].position - vertexPos);
		float dist = length(lights[idx].position - vertexPos);
		attenuation = 1.f / (lights[idx].constant + lights[idx].linear * dist + lights[idx].quadratic * dist * dist);
	} else {
		lightDir = normalize(-lights[idx].position);
	}

	return vec4(lightDir, attenuation);
}


float shadowCalc(float dotNormalLight)
{
	float shadow = 0;
	vec2 texelSize = 1.f / textureSize(shadowMap, 0);

	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;	// unesseccary for orthographic projection
	projCoords = projCoords * .5f + .5f;
	
	float currentDepth = min(projCoords.z, 1.f);
	float bias = max(.005f * (1.f - dotNormalLight), 0.003f);	// moves fragments up slightly to fix shadow acne

	int pcfStart = -4;
	for(int x = pcfStart; x <= -pcfStart; x++) {
		for(int y = pcfStart; y <= -pcfStart; y++) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.f : 0.f;
		}
	}

	int pcfCount = -pcfStart - pcfStart + 1;
	pcfCount *= pcfCount;
	shadow /= pcfCount;

	/*
	float depth = texture(shadowMap, projCoords.xy).r;
	shadow += currentDepth - bias > depth ? 1.f : 0.f;
	*/

	return shadow;
}

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
		vec4 lightDirAndAtten = lightDirection(i);
		vec3 lightDir = vec3(lightDirAndAtten);
		float attenuation = lightDirAndAtten.a;
		
			
		diffuse += max(dot(lightDir, unitNormal), 0.0) * diffuseCoeff * lights[i].color * attenuation;
		vec3 reflect = 2 * dot(lightDir, unitNormal) * unitNormal - lightDir;
		reflect = normalize(reflect);
		specular += pow(max(dot(reflect, viewDir), 0.0), shininess) * specularCoeff * lights[i].color * attenuation;
	}
	float normalLightDot = dot(vec3(lightDirection(0)), unitNormal);	// assume first light is the directional light
	float shadow = shadowCalc(normalLightDot);

	return vec4(ambient + (1.f - shadow) * (diffuse + specular), 1.f);
}

float beckmannNDF(vec3 unitNormal, vec3 midLightCamera)
{
	float alpha = acos(max(dot(unitNormal, midLightCamera), 0));
	float beta = tan(alpha) / roughness;
	float exponent = -beta * beta;
	float num = pow(E, exponent);
	float cosAlpha = cos(alpha);
	float denom = PI * roughness * roughness * cosAlpha * cosAlpha * cosAlpha * cosAlpha + EPSILON;
	return num / denom;
}

float ggxNDF(vec3 unitNormal, vec3 midLightCamera)
{
	float alpha = roughness * roughness;
	float dotNormalMid = max(dot(unitNormal, midLightCamera), 0);
	float b = dotNormalMid * dotNormalMid * (alpha * alpha - 1) + 1;
	return (alpha * alpha) / (PI * b * b);
}

float geometricAttenuation(vec3 unitToLight, vec3 unitToCamera, vec3 unitNormal)
{
	float dotNormalLight = max(dot(unitNormal, unitToLight), 0);
	float dotNormalCamera = max(dot(unitNormal, unitToCamera), 0);
	float k = (roughness + 1) * (roughness + 1) / 8.0f;
	float a = dotNormalLight / (dotNormalLight * (1 - k) + k);
	float b = dotNormalCamera / (dotNormalCamera * (1 - k) + k);
	return a * b;
}

vec3 fresnelReflectance(vec3 unitToCamera, vec3 midLightCamera)
{
	float a = (1 - max(dot(unitToCamera, midLightCamera), 0));
	return fresnel + (1 - fresnel) * a * a * a * a* a;
}

vec3 cookTorrance(vec3 surfaceColor)
{
	vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);

	// Ambient color
	vec3 ambient = vec3(.1f);
	finalColor += ambient;

	vec3 viewDir = normalize(cameraPos - vertexPos);
	vec3 unitNormal = normalize(normal);

	float normalLightDot = dot(vec3(lightDirection(0)), unitNormal);	// assume first light is the directional light
	float shadow = shadowCalc(normalLightDot);

	// Iterate over every light and calculate diffuse and specular
	// components of final color.
	for (int i = 0; i < lightCount; i++)
	{
		vec3 diffuse = vec3(0.0f);
		vec3 specular = vec3(0.0f);

		vec4 lightDirAndAtten = lightDirection(i);
		vec3 lightDir = vec3(lightDirAndAtten);
		float attenuation = lightDirAndAtten.a;

		float angleNormalLight = max(dot(unitNormal, lightDir), 0);
		vec3 lightEnergy = lights[i].color * angleNormalLight * attenuation;
		
		diffuse += diffuseCoeff * surfaceColor/ PI;

		vec3 midLightCamera = normalize(viewDir + lightDir);
		
		// Select from the CPU which NDF to use.
		float dBeckmann = useBeckmann ? beckmannNDF(unitNormal, midLightCamera) : 1.0f;
		float dGGX = useGGX ? ggxNDF(unitNormal, midLightCamera) : 1.0f;

		float g = geometricAttenuation(lightDir, viewDir, unitNormal);
		vec3 f = fresnelReflectance(viewDir, midLightCamera);
		vec3 num = dBeckmann * dGGX * g * f;

		// Don't clamp these dot products to 0 because they are part
		// of the denominator.
		float denom = 4 * dot(unitNormal, lightDir) * dot(viewDir, unitNormal);

		specular += specularCoeff * num / denom;  

		finalColor += lightEnergy * (diffuse + specular) * (1.f - shadow);
	}

	return finalColor;
}

void main()
{
	if (!isEmission)
	{
		vec4 fColor = isInstanceColor ? instanceColor : color;
		
		if (hasTexture)
			fColor = mix(texture(tex, texCoord), fColor, 0.5f);

		if (shadingModel == PHONG) {
			fragColor = phong() * fColor;
		}
		else if (shadingModel == COOK_TORRANCE) {
			fragColor = vec4(cookTorrance(vec3(fColor)), 1.f);
		}
		else {
			// something went wrong.
			fragColor = vec4(1.f, 0, 0, 1.f);
		}
		
	}
	else
	{
		fragColor = color;
	}
}
