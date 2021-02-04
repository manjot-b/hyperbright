#include "PickupManager.h"
#include "Pickup.h"
#include "Vehicle.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

PickupManager::PickupManager() {
	
}

PickupManager::~PickupManager() {

}

/////////////////////////////////////////////////////////////////////////////
//NEEDS A WAY TO DETERMINE PICKUP TYPES
void PickupManager::setupPickups(int numberOfPickups, std::vector <glm::vec3> startingPositions) {
	std::shared_ptr<Pickup> newPickup;
	for (int i = 0; i < numberOfPickups; i++) {
		newPickup = std::shared_ptr<Pickup>(new Pickup());//USE STARTING POSITIONS AND TYPES IN CONSTRUCTOR
		allPickups.push_back(newPickup);
		activePickups.push_back(newPickup);
	}
}

/////////////////////////////////////////////////////////////////////////////

//Check timers on active pickups, deactivate those which are finished
void PickupManager::checkActivePickups() {
	//check time
	for (int i = 0; i < activePickups.size(); i++) {
		if (!activePickups.at(i)->timeRemaining()) {
			//activePickups.at(i)->deactivate();
			inactivePickups.push_back(activePickups.at(i));//Add to inactive
			activePickups.erase(activePickups.begin()+i);//Remove from active
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::removeFromActive(std::shared_ptr<Pickup> pickup) {
	onArenaPickups.erase(std::remove(onArenaPickups.begin(), onArenaPickups.end(), pickup));
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::removeFromArena(std::shared_ptr<Pickup> pickup) {
	onArenaPickups.erase(std::remove(onArenaPickups.begin(), onArenaPickups.end(), pickup));
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::moveToInactive(std::shared_ptr<Pickup> pickup) {
	inactivePickups.push_back(pickup);
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::moveToActive(std::shared_ptr<Pickup> pickup) {
	activePickups.push_back(pickup);
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::moveToArena(std::shared_ptr<Pickup> pickup) {
	onArenaPickups.push_back(pickup);
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::tearDown() {
	onArenaPickups.clear();
	inactivePickups.clear();
	activePickups.clear();
	for (int i = 0; i < allPickups.size(); i++) {
		delete &allPickups.end();
		allPickups.pop_back();
	}
}