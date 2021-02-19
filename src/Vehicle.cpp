#include "Vehicle.h"
#include "Controller.h"
#include "Simulate.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace glm;

Vehicle::Vehicle() {}

Vehicle::Vehicle(shared_ptr<Model> _carModel, vec3 color, vec3 startPos, vec3 startDir = vec3(0.0f, 0.0f, -1.0f))
	: carModel(_carModel), color(color), position(startPos), direction(startDir), startDirection(startDir)
{
	id = carModel->getId();
	if (strcmp(id, "player") == 0) {
		ctrl.contrId = 0;
	}
	else if (strcmp(id, "ai1") == 0) {
		ctrl.contrId = 1;
	}
	else if (strcmp(id, "ai2") == 0) {
		ctrl.contrId = 2;
	}
	else if (strcmp(id, "ai3") == 0) {
		ctrl.contrId = 3;
	}
	else {
		cout << "unknown vehicle name. see vehicle constructor" << endl;
	}
}

void Vehicle::updatePositionAndDirection() {
	position = carModel->getPosition();
	mat4 modelMatrix = carModel->getModelMatrix();
	mat4 rotMatrix = { modelMatrix[0], modelMatrix[1], modelMatrix[2], vec4(0.f, 0.f, 0.f, 1.f) };
	
	direction = normalize(vec3(rotMatrix * vec4(0.f, 0.f, 1.f, 1.0f)));	
}

Vehicle::~Vehicle() {

}

void Vehicle::reset() {
	//Set to begining of game values

}

quat Vehicle::getOrientation()
{
	direction = normalize(direction);
	vec3 worldUp(0.f, 1.f, 0.f);
	vec3 right = normalize(cross(direction, worldUp));
	vec3 up = normalize(cross(right, direction));
	mat4 m = lookAt(vec3(0.f), direction, up);

	return quat_cast(m);
}

void Vehicle::accelerateForward()
{
	ctrl.input[0] = 1;
}

void Vehicle::accelerateReverse()
{
	ctrl.input[1] = 1;
}

void Vehicle::brake()
{
	ctrl.input[5] = 1;
}

void Vehicle::turnLeft()
{
	ctrl.input[3] = 1;
}

void Vehicle::turnRight()
{
	ctrl.input[2] = 1;
}

void Vehicle::turnHardLeft()
{
	ctrl.input[4] = 1;
	turnLeft();
}

void Vehicle::turnHardRight()
{
	ctrl.input[4] = 1;
	turnRight();
}

void Vehicle::resetControls()
{
	for (int i = 0; i < 6; i++) {
		ctrl.input[i] = 0;
	}
}

void Vehicle::stopForward()
{
	ctrl.input[0] = 0;
}

void Vehicle::stopReverse()
{
	ctrl.input[1] = 0;
}

void Vehicle::stopLeft()
{
	ctrl.input[3] = 0;
}

void Vehicle::stopRight()
{
	ctrl.input[2] = 0;
}


