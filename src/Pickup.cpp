#include "Pickup.h"
#include "Vehicle.h"
#include <GLFW/glfw3.h>
#include <iostream>

Pickup::Pickup() {
	type = 0;//DEFAULT
	active = false;
}

Pickup::~Pickup() {

}

Pickup::Pickup(int pickupType) {
	type = pickupType;
	active = false;
	beingCarried = false;
	slowTrapActive = false;
	pickupTime = 5.f;
	//position
}

/////////////////////////////////////////////////////////////////////

void Pickup::activate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace) {
	std::cout << "ACTIVATED\n";
	if (type == SPEED) {
		//SET NEW VEHICLE MAX SPEED
		pickUpStartTime = glfwGetTime();
	}
	else if (type == ZAP) {
		zapOldColor = vehicles[indexOfFirstPlace].color;
		vehicles[indexOfFirstPlace].color = vehicles[indexOfActivator].color;
		pickUpStartTime = glfwGetTime();
	}
	else if (type == EMP) {
		vehicles[0].energy = 0;
		vehicles[1].energy = 0;
		vehicles[2].energy = 0;
		vehicles[3].energy = 0;
		//TEAR DOWN
	}
	else if (type == HIGHVOLTAGE) {
		//CHANGE COLOR LAYING AREA OF vehicles[indexOfActivator]
		pickUpStartTime = glfwGetTime();
	}
	else if (type == SLOWTRAP) {
		//SET CURRENT POSITION TO BEHIND vehicles[indexOfActivator]
		beingCarried = false;
		slowTrapActive = true;
	}
	else if (type == SUCKER) {
		vehicles[indexOfActivator].suckerActive = true;
		pickUpStartTime = glfwGetTime();
	}
	else if (type == SYPHON) {
		vehicles[indexOfActivator].syphonActive = true;
		pickUpStartTime = glfwGetTime();
	}

	return;
}

/////////////////////////////////////////////////////////////////////

void Pickup::deactivate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace) {
	std::cout << "DEACTIVATED\n";
	if (type == SPEED) {
		//SET OLD VEHICLE MAX SPEED
		//TEAR DOWN
	}
	else if (type == ZAP) {
		vehicles[indexOfFirstPlace].color = zapOldColor;
		//TEAR DOWN
	}
	else if (type == HIGHVOLTAGE) {
		//CHANGE COLOR LAYING AREA OF vehicles[indexOfActivator] TO ORIGINAL
		//TEAR DOWN
	}
	else if (type == SUCKER) {
		vehicles[indexOfActivator].suckerActive = false;
		//TEAR DOWN
	}
	else if (type == SYPHON) {
		vehicles[indexOfActivator].syphonActive = false;
		//TEAR DOWN
	}
	return;
}

/////////////////////////////////////////////////////////////////////

void Pickup::initialCollision(Vehicle vehicle, int pickupIndex) {
	if (type == BATTERY) {
		vehicle.energy += 50;//BATTERY ACTIVATE
		//TEAR DOWN
	}
	else if (type == SLOWTRAP && slowTrapActive) {
		//TO DO, IF YOU HIT THE ACTIVE SLOWTRAP
	} else {
		vehicle.pickupIndex = pickupIndex;
		beingCarried = true;
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
