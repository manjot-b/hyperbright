#pragma once

#include <glm/glm.hpp>

#include "Vehicle.h"

class Ai
{
public:
	Ai(std::shared_ptr<Vehicle> v);
	~Ai();
	void aiInput();
private:
	std::shared_ptr<Vehicle> vehicle;
};
