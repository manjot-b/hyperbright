#pragma once

#include <glm/glm.hpp>

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
	glm::vec4 color;
	float diffuse;
	float specular;
	float shininess;
};