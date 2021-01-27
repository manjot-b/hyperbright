#pragma once
class Pickup;//forward declaration
#include <string>
#include <glm/glm.hpp>
#include <memory>

//#include "Pickup.h"

class Vehicle
{
public:
	Vehicle();
	~Vehicle();
	void reset();
	int energy;

	Pickup * pickupEquiped;//set as null for default
	int color; //CHANGE TYPE LATER
	bool suckerActive;//IMPLEMENTATION IN COLLISION DETECTION 
	bool syphonActive;//IMPLEMENTATION IN COLLISION DETECTION
private:

};
