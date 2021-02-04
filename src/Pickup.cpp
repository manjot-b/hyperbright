#include "Pickup.h"
#include "Vehicle.h"
#include "PickupManager.h"
#include <GLFW/glfw3.h>
#include <iostream>

Pickup::Pickup() {
	type = 0;//DEFAULT
	active = false;
}

Pickup::~Pickup() {

}

Pickup::Pickup(int pickupType, std::shared_ptr<PickupManager> pickupMan ) {
	pickupManager = pickupMan;
	type = pickupType;
	active = false;
	//beingCarried = false;
	slowTrapActive = false;
	pickupTime = 5.f;
	//position
}

/////////////////////////////////////////////////////////////////////
/*
This function is called when the user activates the power up after
they have picked it up. Makes changes to the appropriate entities
based on which type of power up it is.
*/
void Pickup::activate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace) {
	std::cout << "ACTIVATED\n";
	if (type == SPEED) {
		//SET NEW VEHICLE MAX SPEED
		//move to active
		pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == ZAP) {
		zapOldColor = vehicles[indexOfFirstPlace].color;
		vehicles[indexOfFirstPlace].color = vehicles[indexOfActivator].color;
		//move to active
		pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == EMP) {
		vehicles[0].energy = 0;
		vehicles[1].energy = 0;
		vehicles[2].energy = 0;
		vehicles[3].energy = 0;
		//TEAR DOWN
		//move to inactive
		pickupManager->moveToInactive(std::shared_ptr<Pickup>(this));
	}
	else if (type == HIGHVOLTAGE) {
		//CHANGE COLOR LAYING AREA OF vehicles[indexOfActivator]
		//move to active
		pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == SLOWTRAP) {
		//SET CURRENT POSITION TO BEHIND vehicles[indexOfActivator]
		//move to onArena
		pickupManager->moveToArena(std::shared_ptr<Pickup>(this));
		//beingCarried = false;
		slowTrapActive = true;
	}
	else if (type == SUCKER) {
		vehicles[indexOfActivator].suckerActive = true;
		//move to active
		pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}
	else if (type == SYPHON) {
		vehicles[indexOfActivator].syphonActive = true;
		//move to active
		pickupManager->moveToActive(std::shared_ptr<Pickup>(this));

		pickUpStartTime = glfwGetTime();
	}

	return;
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
		vehicles[indexOfFirstPlace].color = zapOldColor;
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
	pickupManager->removeFromActive(std::shared_ptr<Pickup>(this));
	pickupManager->moveToInactive(std::shared_ptr<Pickup>(this));
	return;
}

/////////////////////////////////////////////////////////////////////
/*
This function is called when given vehicle collides with this powerup.
*/
void Pickup::initialCollision(std::shared_ptr<Vehicle> vehicle) {
	if (type == BATTERY) {
		vehicle->energy += 50;//BATTERY ACTIVATE
		//move from onArena to inactive
		pickupManager->removeFromArena(std::shared_ptr<Pickup>(this));
		pickupManager->moveToInactive(std::shared_ptr<Pickup>(this));
	}
	else if (type == SLOWTRAP && slowTrapActive) {
		//TO DO, IF YOU HIT THE ACTIVE SLOWTRAP
		//move from onArena to active
		pickupManager->removeFromArena(std::shared_ptr<Pickup>(this));
		pickupManager->moveToActive(std::shared_ptr<Pickup>(this));
	} else {
		vehicle->pickupEquiped = std::shared_ptr<Pickup>(this);
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
