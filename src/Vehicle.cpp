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

quat Vehicle::getOrientation()
{
	forward = normalize(forward);
	vec3 v(0.f, 1.f, 0.f);
	vec3 u = normalize(cross(forward, v));
	vec3 normal = normalize(cross(u, forward));
	mat4 m = lookAt(vec3(0.f), forward, normal);

	return quat_cast(m);
}
