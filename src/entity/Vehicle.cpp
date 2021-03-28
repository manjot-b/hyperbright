#include "Vehicle.h"

#include <vehicle/PxVehicleDrive4W.h>
#include <glm/gtx/euler_angles.hpp>

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
	direction(normalize(startDir)),
	up(vec3(0.f, 1.f, 0.f)), // in order for 'up' to update properly, vehicle must start in upright position
	right(cross(direction, up)),
	lastPosition(startPos)
{
	setTriggerType(physics::IPhysical::TriggerType::VEHICLE);
	string bodyIdSuffix = "body";
	string wheelsIdSuffix = "wheel";
	syphonActive = false;
	switch (team)
	{
	case teamStats::Teams::TEAM0:
		ctrl.contrId = 0;
		break;
	case teamStats::Teams::TEAM1:
		teamNum = 1;
		ctrl.contrId = 1;
		break;
	case teamStats::Teams::TEAM2:
		teamNum = 2;
		ctrl.contrId = 2;
		break;
	case teamStats::Teams::TEAM3:
		teamNum = 3;
		ctrl.contrId = 3;
		break;
	default:
		cout << "unknown vehicle name. see vehicle constructor" << endl;
		break;
	}

	body = std::make_unique<model::Model>("rsc/models/car_body.obj", teamStats::names[team] + bodyIdSuffix, _shader, nullptr);
	body->setPosition(startPos);

	
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
			brakeLightsColor = mesh->material.color;
			brakeLightsIdx = index;
		}

		// brake lights color comes from car_model.obj
		// set reverse lights color manually
		reverseLightsColor = vec4(1.f, .75f, .9f, 1.f);

		index++;
	}

	wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = std::make_unique<model::Model>("rsc/models/wheel.obj", teamStats::names[team] + wheelsIdSuffix + "_fl", _shader, nullptr);
	wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = std::make_unique<model::Model>("rsc/models/wheel.obj", teamStats::names[team] + wheelsIdSuffix + "_fr", _shader, nullptr);
	wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] = std::make_unique<model::Model>("rsc/models/wheel.obj", teamStats::names[team] + wheelsIdSuffix + "_rl", _shader, nullptr);
	wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = std::make_unique<model::Model>("rsc/models/wheel.obj", teamStats::names[team] + wheelsIdSuffix + "_rr", _shader, nullptr);
}

void Vehicle::updateOrientation() {
	mat4 modelMatrix = body->getModelMatrix();
	// zero out the translation
	mat4 rotMatrix = { modelMatrix[0], modelMatrix[1], modelMatrix[2], vec4(0.f, 0.f, 0.f, 1.f) };
	direction = normalize(vec3(rotMatrix * vec4(0.f, 0.f, 1.f, 1.f)));	
	up = normalize(vec3(rotMatrix * vec4(0.f, 1.f, 0.f, 1.f)));
	right = normalize(vec3(rotMatrix * vec4(1.f, 0.f, 0.f, 1.f)));

	upright = (up.y > 0) ? true : false;
}

Vehicle::~Vehicle() {

}

void Vehicle::reset() {
	//Set to begining of game values

}

void Vehicle::render() const
{
	body->render();
	for (const auto& wheel : wheels)
		wheel->render();
}

void Vehicle::renderShadow(const std::shared_ptr<openGLHelper::Shader>& shadowShader) const
{
	body->renderShadow(shadowShader);
	for (const auto& wheel : wheels)
		wheel->renderShadow(shadowShader);
}

quat Vehicle::getOrientation() const
{
	return quat_cast(lookAt(vec3(0.f), direction, up));
}

void Vehicle::reduceEnergy()
{
	if (energy > 0) energy -= 0.04;
}

void Vehicle::increaseEnergy()
{
	energy += 0.02;
}

void Vehicle::restoreEnergy()
{
	energy = 1.f;////////////////
}

bool Vehicle::enoughEnergy()
{
	return (energy > 0) ? true : false;
}

bool Vehicle::fullEnergy()
{
	return (energy >= 1.f) ? true : false;
}

void Vehicle::equipPickup(std::shared_ptr<Pickup> _pickup)
{
	pickup = _pickup;
	pickupEquiped = true;
}

void Vehicle::activatePickup()
{
	if (pickupEquiped) {
		pickup->use(team);
		pickup = nullptr;
		pickupEquiped = false;
	}
}

void Vehicle::applyFlipImpulse()
{
	ctrl.flipImpulse = true;
}

void Vehicle::applyBoost(int duration)
{
	ctrl.boost.first = duration;
	ctrl.boost.second = true;
}

void Vehicle::releaseBoost()
{
	ctrl.boost.second = false;
}

void Vehicle::applyTrap(int duration)
{
	ctrl.trap.first = duration;
	ctrl.trap.second = true;
}

void Vehicle::releaseTrap()
{
	ctrl.trap.second = false;
}

void Vehicle::accelerateForward()
{
	ctrl.input[0] = 1;
}

//void engine::teamStats::Teams setTeam(engine::teamStats::Teams t) { team = t; return; }

void Vehicle::accelerateReverse()
{
	body->getMeshes()[brakeLightsIdx]->material.isEmission = true;
	if (speedometer > 2.f) {
		brake();
	}
	else {
		body->getMeshes()[brakeLightsIdx]->material.color = reverseLightsColor;
		stopBrake();
		ctrl.input[1] = 1;
	}
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

void Vehicle::hardTurn()
{
	ctrl.input[4] = 1;
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
	ctrl.input[5] = 0;
	ctrl.input[1] = 0;
	body->getMeshes()[brakeLightsIdx]->material.isEmission = false;
	body->getMeshes()[brakeLightsIdx]->material.color = brakeLightsColor;
}

void Vehicle::stopBrake() 
{
	ctrl.input[5] = 0;
}

void Vehicle::stopLeft()
{
	ctrl.input[3] = 0;
}

void Vehicle::stopRight()
{
	ctrl.input[2] = 0;
}

void Vehicle::stopHardTurn() 
{
	ctrl.input[4] = 0;
}

void Vehicle::updateSpeedometer(float deltaTime)
{
	vec3 travelled = body->getPosition() - lastPosition;
	float projOnDirection = dot(travelled, direction);
	if (projOnDirection >= 0) {
		speedometer = length(travelled) / deltaTime;
	}
	else {
		speedometer = -length(travelled) / deltaTime;
	}
}

void Vehicle::setModelMatrix(const glm::mat4& modelMat)
{
	// Probably a better way to do this, but this is fine for now.
	float scale = 1 / 3.f; // this must match physX vehicle description in Simulate.cpp - initVehicleDesc()
	glm::vec3 translate(0.f, -1.7f, 0.f);	// do to the scaling down, we must translate the model down to rest on the ground plane

	glm::mat4 final_transform = modelMat;
	final_transform = glm::scale(modelMat, glm::vec3(scale));
	final_transform = glm::translate(final_transform, translate);
	body->setModelMatrix(final_transform);
}

void Vehicle::setWheelsModelMatrix(const glm::mat4& frontLeft, const glm::mat4& frontRight, const glm::mat4& rearRight, const glm::mat4& rearLeft)
{
	glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(1 / 3.f)); // this must match physX vehicle description in Simulate.cpp - initVehicleDesc()
	glm::mat4 flipped = glm::eulerAngleY(glm::radians(180.f));

	wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT]->setModelMatrix(frontLeft * scale);
	wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT]->setModelMatrix(frontRight * scale * flipped);
	wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT]->setModelMatrix(rearLeft * scale);
	wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT]->setModelMatrix(rearRight * scale * flipped);
}

void Vehicle::setPosition(const glm::vec3& _position)
{
	lastPosition = body->getPosition();
	body->setPosition(_position);

	for(auto& wheel : wheels)
		wheel->setPosition(_position);
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