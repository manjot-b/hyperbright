#include "Pickup.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "PickupManager.h"
#include "Vehicle.h"

namespace hyperbright {
namespace entity {

Pickup::Pickup() {
	pickupNumber = 0;
	setTriggerTrype(physics::IPhysical::TriggerType::PICKUP);
}

Pickup::Pickup( const std::shared_ptr<openGLHelper::Shader>& shader) : IRenderable(shader),
	model(std::make_shared<model::Model>("rsc/models/powerup.obj", "pickup", shader, nullptr)) {
	setTriggerTrype(physics::IPhysical::TriggerType::PICKUP);

	type = 0;//DEFAULT
	active = false;
	pickupNumber = 0;

	tile = glm::vec2(0,0);
	//pickupNumber = puNum;
	//setArenaLocation(glm::vec3(0.f, 0.f, 3.f));
}

Pickup::~Pickup() {

}

Pickup::Pickup(int puNum, int pickupType, std::shared_ptr<PickupManager> pickupMan, const std::shared_ptr<openGLHelper::Shader>& shader) : Pickup(shader) {

	pickupManager = pickupMan;
	type = pickupType;
	active = false;
	beingCarried = false;
	slowTrapActive = false;
	pickupNumber = puNum;
	pickupTime = 5.f;
	//position
	if (type == STATION) {
		// station model not  yet made
	}
	else {
		model = std::make_shared<model::Model>("rsc/models/powerup.obj", "pickup", shader, nullptr);
	}
}

void Pickup::activate() {
	pickUpStartTime = glfwGetTime();
}

/////////////////////////////////////////////////////////////////////
/*
This function is called when the user activates the power up after
they have picked it up. Makes changes to the appropriate entities
based on which type of power up it is.
*/
void Pickup::activate(std::vector<std::shared_ptr<entity::Vehicle>>* vehicles, int indexOfFirstPlace) {
	std::cout << "ACTIVATED\n";
	if (type == SPEED) {
		//SET NEW VEHICLE MAX SPEED
		//move to active
		//pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == ZAP) {
		//zapOldColor = vehicles[indexOfFirstPlace].getColor();
		//vehicles[indexOfFirstPlace].setColor(vehicles[indexOfActivator].getColor());
		//move to active
		//pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == EMP) {
		vehicles->at(0)->energy = 0;
		vehicles->at(1)->energy = 0;
		vehicles->at(2)->energy = 0;
		vehicles->at(3)->energy = 0;
		//TEAR DOWN
		//move to inactive
		//pickupManager->moveToInactive(std::shared_ptr<Pickup>(this));
	}
	else if (type == HIGHVOLTAGE) {
		//CHANGE COLOR LAYING AREA OF vehicles[indexOfActivator]
		//move to active
		//pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == SLOWTRAP) {
		//SET CURRENT POSITION TO BEHIND vehicles[indexOfActivator]
		//move to onArena
		//pickupManager->moveToArena(std::shared_ptr<Pickup>(this));
		//beingCarried = false;
		slowTrapActive = true;
	}
	else if (type == SUCKER) {
		//vehicles[indexOfActivator].suckerActive = true;
		//move to active
		//pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == SYPHON) {
		//vehicles[indexOfActivator].syphonActive = true;
		//move to active
		//pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}

	return;
}

/////////////////////////////////////////////////////////////////////

void Pickup::use(int indexOfUser) {
	beingCarried = false;
	active = true;
	usedByIndex = indexOfUser;
}

/////////////////////////////////////////////////////////////////////

void Pickup::deactivate() {

}

/////////////////////////////////////////////////////////////////////
/*
This function is called when the power up timer is up. It undos whatever
was done during the activation. 
*/
void Pickup::deactivate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace) {
	std::cout << "DEACTIVATED\n";
	if (type == SPEED) {
		//SET OLD VEHICLE MAX SPEED
	}
	else if (type == ZAP) {
		vehicles[indexOfFirstPlace].setColor(zapOldColor);
	}
	else if (type == HIGHVOLTAGE) {
		//CHANGE COLOR LAYING AREA OF vehicles[indexOfActivator] TO ORIGINAL
	}
	else if (type == SUCKER) {
		vehicles[indexOfActivator].suckerActive = false;
	}
	else if (type == SYPHON) {
		vehicles[indexOfActivator].syphonActive = false;
	}
	return;
}

/////////////////////////////////////////////////////////////////////
/*
This function is called when given vehicle collides with this powerup.
*/
void Pickup::initialCollision(std::shared_ptr<Vehicle> vehicle) {
	 if (type == SLOWTRAP && slowTrapActive) {
		//TO DO, IF YOU HIT THE ACTIVE SLOWTRAP
		//move from onArena to active
		pickupManager->removeFromArena(std::shared_ptr<Pickup>(this));
		pickupManager->moveToActive(std::shared_ptr<Pickup>(this));
	} else {
		vehicle->equipPickup(std::shared_ptr<Pickup>(this));
		//remove from onArena
		pickupManager->removeFromArena(std::shared_ptr<Pickup>(this));
	}
	return;
}

/////////////////////////////////////////////////////////////////////

bool Pickup::timeRemaining() {
	if (glfwGetTime() - pickUpStartTime > pickupTime) {
		return true;
	}
		return false;
}

void Pickup::render() const {
	model->render();	// make sure we have set the models shader.
}

void Pickup::animate(float deltaSec) {
	const float rotSpeed = glm::radians(180.f) * deltaSec;
	model->rotate(glm::vec3(0, rotSpeed, 0));
	model->update();

	// This is hacky but it works. Set the y-pos directly in the model matrix.
	const float transSpeed = 5.f;
	const float maxPos = 0.5f;
	const float newPos = (glm::cos(transSpeed * glfwGetTime()) + 1) * 0.5f * maxPos;
	glm::mat4 modelMat = model->getModelMatrix();
	glm::vec4 pos = modelMat[3];
	pos.y = newPos;
	modelMat[3] = pos;
	//modelMat = glm::translate(modelMat, arenaLocation);
	model->setModelMatrix(modelMat);
}

void Pickup::setArenaLocation(glm::vec3 _arenaLocation, std::optional<glm::vec2> tileLocation)
{
	arenaLocation = _arenaLocation;
	tile = glm::vec2(tileLocation->x, tileLocation->y);
	//std::cout << "pu made at: " << tile.x << " " << tile.y << "\n";
	model->translate(_arenaLocation);
	model->update();
}

void Pickup::setModelMatrix(const glm::mat4& modelMat) { model->setModelMatrix(modelMat); }
void Pickup::setPosition(const glm::vec3& position) { model->setPosition(position); }

}	// namespace entity
}	// namespace hyperbright