#pragma once
#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Pickup.h"

class Vehicle
{
public:
	Vehicle();
	~Vehicle();
	void reset();
	int energy;
	int color; //CHANGE TYPE LATER
	bool suckerActive;//IMPLEMENTATION IN COLLISION DETECTION 
	bool syphonActive;//IMPLEMENTATION IN COLLISION DETECTION
	Pickup pickupEquiped;//set as null for default
private:
};
