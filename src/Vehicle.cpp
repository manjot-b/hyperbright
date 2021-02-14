#include "Vehicle.h"
#include "Controller.h"
#include <iostream>

using namespace std;
using namespace glm;

Vehicle::Vehicle() {}

Vehicle::Vehicle(shared_ptr<Model> _carModel, vec3 color, vec3 startPos, vec3 startDir = vec3(0.0f, 0.0f, -1.0f))
	: carModel(_carModel), color(color), start_position(startPos), forward(startDir)
{
	id = carModel->getId();
}

Vehicle::~Vehicle() {

}

void Vehicle::reset() {
	//Set to begining of game values

}