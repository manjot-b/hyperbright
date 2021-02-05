#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <queue>
#include "Pickup.h"
#include "Model.h"

class Vehicle
{
public:
	Vehicle();
	Vehicle(std::shared_ptr<Model> carModel, glm::vec3 startDir);
	~Vehicle();
	void reset();


	void drive(std::queue<int> inputs);
	float getSpeed() { return speed; }
	glm::vec3 getForward() { return forward; }
	void resetAngle() { theta = M_PI / 2; }

	int energy;
	int color; //CHANGE TYPE LATER
	bool suckerActive;//IMPLEMENTATION IN COLLISION DETECTION 
	bool syphonActive;//IMPLEMENTATION IN COLLISION DETECTION
	Pickup pickupEquiped;//set as null for default

	// Movement calls for vehicle
	// accelerate
	// brake
	// turn left
	// turn left hard
	// turn right 
	// turn right hard
private:
	void updateForward();

	std::shared_ptr<Model> carModel;

	float speed;
	float turnSpd;
	glm::vec3 forward;

	float targetAngle;
	float theta;
};
