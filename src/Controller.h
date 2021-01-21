#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Vehicle.h"
#include "Ai.h"

class Controller
{
public:
	Controller();
	~Controller();
	void gameInput(Vehicle vehicle);
private:

};
