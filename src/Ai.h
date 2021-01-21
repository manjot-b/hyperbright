#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Vehicle.h"

class Ai
{
public:
	Ai();
	~Ai();
	void reset();
	void aiInput(Vehicle vehicle);
private:

};
