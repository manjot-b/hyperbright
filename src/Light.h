#pragma once

#include <glm/glm.hpp>

struct Light
{
	bool isPoint;
	glm::vec3 position;	// represents the direction if not point light.
	glm::vec3 color;

	// used for attenuation of point lights.
	float constant;
	float linear;
	float quadratic;
};