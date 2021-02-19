#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <array>
#include <glm/gtc/quaternion.hpp>
#include "Pickup.h"
#include "Model.h"


using namespace std;
using namespace glm;

struct VehicleController {
	int contrId;
	int input[6] = { 0,0,0,0,0,0 };
};

class Vehicle
{
public:
	Vehicle();
	Vehicle(shared_ptr<Model> carModel, vec3 color, vec3 startPos, vec3 startDir);
	~Vehicle();
	void reset();

	const char* getId() { return id; }
	VehicleController getController() { return ctrl; }
	vec3 getColor() { return color; }
	vec3 getForward() { return forward; }
	const vec3 getStartPos() { return start_position; }
	quat getOrientation();

	float energy;
	bool suckerActive;//IMPLEMENTATION IN COLLISION DETECTION 
	bool syphonActive;//IMPLEMENTATION IN COLLISION DETECTION

	shared_ptr<Pickup> pickupEquiped;//set as null for default
	shared_ptr<Model> carModel;

	// driving movements act as a toggle. call the appropriate movement 
	// function to start moving then call the cooresponding stop function 
	void accelerateForward();
	void accelerateReverse();
	void brake();		// not implemented
	void turnLeft();
	void turnRight();
	void turnHardLeft();// not implemented
	void turnHardRight();// not implemented
	void resetControls();

	void stopForward();
	void stopReverse();
	void stopLeft();
	void stopRight();
private:

	const char* id;
	vec3 color;
	vec3 forward;
	const vec3 start_position;
	VehicleController ctrl;
};
