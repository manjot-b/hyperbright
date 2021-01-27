#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>


class Vehicle
{
public:
	Vehicle();
	~Vehicle();
	void reset();
	int energy;
	int pickupIndex;
	int color; //CHANGE TYPE LATER
	bool suckerActive;//IMPLEMENTATION IN COLLISION DETECTION 
	bool syphonActive;//IMPLEMENTATION IN COLLISION DETECTION
private:

};
