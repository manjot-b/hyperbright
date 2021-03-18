#include "Vehicle.h"

#include <iostream>
#include <algorithm>

namespace hyperbright {
namespace entity {

using namespace std;
using namespace glm;
using namespace engine;

Vehicle::Vehicle(
	teamStats::Teams team,
	const std::shared_ptr<openGLHelper::Shader>& shader,
	vec3 startPos,
	vec3 startDir = vec3(0.0f, 0.0f, -1.0f))
	:IRenderable(shader),
	team(team), color(teamStats::colors.at(team)),
	position(startPos), direction(normalize(startDir)), startDirection(startDir)
{
	string bodyIdSuffix = "body";
	string wheelsIdSuffix = "wheel";

	switch (team)
	{
	case teamStats::Teams::TEAM0:
		ctrl.contrId = 0;
		break;
	case teamStats::Teams::TEAM1:
		ctrl.contrId = 1;
		break;
	case teamStats::Teams::TEAM2:
		ctrl.contrId = 2;
		break;
	case teamStats::Teams::TEAM3:
		ctrl.contrId = 3;
		break;
	default:
		cout << "unknown vehicle name. see vehicle constructor" << endl;
		break;
	}

	body = std::make_unique<model::Model>("rsc/models/car_body.obj", teamStats::names[team] + bodyIdSuffix, _shader, nullptr);
	
	unsigned int index = 0;
	for (auto& mesh : body->getMeshes())
	{
		// The name of the material is "body".
		if (mesh->getName() == "body")
		{
			bodyIdx = index;
			mesh->material.color = color;
			mesh->material.shadingModel = model::Material::ShadingModel::COOK_TORRANCE;
			mesh->material.roughness = 0.2f;
			mesh->material.useBeckmann = true;
			mesh->material.useGGX = false;
		}
		else if (mesh->getName() == "front_lights")
		{
			mesh->material.isEmission = true;
		}
		else if (mesh->getName() == "rear_lights")
		{
			brakeLightsIdx = index;
		}

		index++;
	}

	wheel = std::make_unique<model::Model>("rsc/models/wheel.obj", id + wheelsIdSuffix, _shader, nullptr);
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

void Vehicle::render() const
{
	body->render();
	wheel->render();
}

quat Vehicle::getOrientation() const
{
	vec3 worldUp(0.f, 1.f, 0.f);
	vec3 right = normalize(cross(direction, worldUp));
	vec3 up = normalize(cross(right, direction));
	mat4 m = lookAt(vec3(0.f), direction, up);

	return quat_cast(m);
}

void Vehicle::reduceEnergy()
{
	if (energy > 0) energy -= 0.04;
}

void Vehicle::restoreEnergy()
{
	energy = 1.f;
}

bool Vehicle::enoughEnergy()
{
	return (energy > 0) ? true : false;
}

void Vehicle::accelerateForward()
{
	ctrl.input[0] = 1;
}

void Vehicle::accelerateReverse()
{
	ctrl.input[1] = 1;

	// TO-DO: Implement this feature in braking instead. Currently the brake lights
	// turn on even if the vehicle is reversing, not only braking.
	body->getMeshes()[brakeLightsIdx]->material.isEmission = true;
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
	body->getMeshes()[brakeLightsIdx]->material.isEmission = false;
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
	float scale = 1 / 3.f; // this must match physX vehicle description in Simulate.cpp - initVehicleDesc()
	glm::vec3 translate(0.f, -2.0f, 0.f);

	glm::mat4 final_transform = modelMat;
	final_transform = glm::scale(modelMat, glm::vec3(scale));
	final_transform = glm::translate(final_transform, translate);
	body->setModelMatrix(final_transform);

	wheel->setModelMatrix(glm::translate(final_transform, glm::vec3(1.5f, 0.51f, 3.0f)));
}

void Vehicle::setPosition(const glm::vec3& position)
{
	this->position = position;
	body->setPosition(position);
	wheel->setPosition(position);
}

void Vehicle::setBodyMaterial(const model::Material& material)
{
	body->getMeshes()[bodyIdx]->material = material;
}

const model::Material& Vehicle::getBodyMaterial() const
{
	return body->getMeshes()[bodyIdx]->material;
}
}	// namespace entity
}	// namespace hyperbright