#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Vehicle.h"

class Controller
{
public:
	Controller();
	~Controller();
	void gameInput(Vehicle vehicles[]);
private:

};
