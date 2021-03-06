#pragma once

#include <glm/glm.hpp>

struct Light
{
	bool isPoint;
	glm::vec3 position;	// represents the direction if not point light.
};