#pragma once

#include <glm/glm.hpp>

namespace hyperbright {
namespace model {
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture;
};

struct BoundingBox
{
	float x, y, z, width, height, depth;
};

struct Material
{
	enum class ShadingModel {
		PHONG = 0,
		COOK_TORRANCE
	} shadingModel;

	glm::vec4 color;
	float diffuse;
	float specular;
	float shininess;
	bool isEmission;
	float reflectiveStrength;

	// These values are used for cook-torrance shading.
	float roughness;
	glm::vec3 fresnel;
	bool useBeckmann;
	bool useGGX;
};
}	// namespace model
}	// namespace hyperbright