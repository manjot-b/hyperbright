#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Vehicle.h"
#include "Arena.h"
#include "Pickup.h"

class Simulate
{
public:
	Simulate();
	~Simulate();
	//void simulateStep(Vehicle vehicles[], Arena arena, Pickup pickups[]);
	void stepPhysics();

private:
	void initPhysics();
};
