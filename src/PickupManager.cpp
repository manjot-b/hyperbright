#include "PickupManager.h"
#include "Pickup.h"
#include "Vehicle.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

PickupManager::PickupManager() {
	//
}

PickupManager::~PickupManager() {

}

void PickupManager::setupPickups() {
	//need starting positions
}

void PickupManager::checkActivePickups() {

}

void PickupManager::removeFromActive(std::shared_ptr<Pickup> pickup) {
	onArenaPickups.erase( std::remove(onArenaPickups.begin(), onArenaPickups.end(), pickup) );
}

void PickupManager::moveToInactive(std::shared_ptr<Pickup> pickup) {
	inactivePickups.push_back(pickup);
}

void PickupManager::moveToActive(std::shared_ptr<Pickup> pickup) {
	activePickups.push_back(pickup);
}

void PickupManager::moveToArena(std::shared_ptr<Pickup> pickup) {
	onArenaPickups.push_back(pickup);
}