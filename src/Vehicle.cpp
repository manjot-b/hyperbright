#include "Vehicle.h"

#include <iostream>
#include <algorithm>

using namespace std;
using namespace glm;

Vehicle::Vehicle(const std::string& _id, vec4 color, vec3 startPos, vec3 startDir = vec3(0.0f, 0.0f, -1.0f))
	: id(_id), color(color), position(startPos), direction(normalize(startDir)), startDirection(startDir)
{
	string bodyIdSuffix = "body";
	string wheelsFrontIdSuffix = "wheelsfront";
	string wheelsRearIdSuffix = "wheelsRear";

	if (id ==  "player") {
		ctrl.contrId = 0;
	}
	else if (id == "ai1") {
		ctrl.contrId = 1;
	}
	else if (id == "ai2") {
		ctrl.contrId = 2;
	}
	else if (id == "ai3") {
		ctrl.contrId = 3;
	}
	else {
		cout << "unknown vehicle name. see vehicle constructor" << endl;
	}

	body = std::make_unique<Model>("rsc/models/car_body.obj", id + bodyIdSuffix, nullptr, color);
	wheelsFront = std::make_unique<Model>("rsc/models/wheels_front.obj", id + wheelsFrontIdSuffix, nullptr, vec4(.1f, .1f, .1f, 1));
	wheelsRear = std::make_unique<Model>("rsc/models/wheels_rear.obj", id + wheelsRearIdSuffix, nullptr, vec4(.1f, .1f, .1f, 1));
}

void Vehicle::updatePositionAndDirection() {
	position = body->getPosition();
	mat4 modelMatrix = body->getModelMatrix();
	mat4 rotMatrix = { modelMatrix[0], modelMatrix[1], modelMatrix[2], vec4(0.f, 0.f, 0.f, 1.f) };
	currentTile = glm::vec2(-1,-1);
	direction = normalize(vec3(rotMatrix * vec4(0.f, 0.f, 1.f, 1.0f)));	
}

Vehicle::~Vehicle() {

}

void Vehicle::reset() {
	//Set to begining of game values

}

void Vehicle::render(const Shader& shader) const
{
	body->render(shader);
	wheelsFront->render(shader);
	wheelsRear->render(shader);
}

quat Vehicle::getOrientation() const
{
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

void Vehicle::setModelMatrix(const glm::mat4& modelMat)
{
	// Probably a better way to do this, but this is fine for now.
	float scale = 0.45f;
	glm::mat4 scaled = modelMat;
	scaled = glm::scale(modelMat, glm::vec3(scale));
	body->setModelMatrix(scaled);
	wheelsFront->setModelMatrix(scaled);
	wheelsRear->setModelMatrix(scaled);
}

void Vehicle::setPosition(const glm::vec3& position)
{
	this->position = position;
	body->setPosition(position);
	wheelsFront->setPosition(position);
	wheelsRear->setPosition(position);
}