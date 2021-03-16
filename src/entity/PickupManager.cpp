#include "PickupManager.h"

#include "Pickup.h"
#include "Vehicle.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
glm::vec2 a = glm::vec2(3, 3);
glm::vec2 b = glm::vec2(20, 3);
glm::vec2 c = glm::vec2(36, 3);
glm::vec2 d = glm::vec2(3, 20);
glm::vec2 e = glm::vec2(20, 20);
glm::vec2 f = glm::vec2(36, 20);
glm::vec2 g = glm::vec2(3, 36);
glm::vec2 h = glm::vec2(20, 36);
glm::vec2 i = glm::vec2(36, 36);
namespace hyperbright {
namespace entity {
PickupManager::PickupManager() {
}

PickupManager::~PickupManager() {
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::setUp(const std::shared_ptr<entity::Arena>& arena) {
	onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(3, 3)) + glm::vec3(0.f, 1.f, 0.f));
	onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(36, 3)) + glm::vec3(0.f, 1.f, 0.f));
	onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(3, 36)) + glm::vec3(0.f, 1.f, 0.f));
	onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(36, 36)) + glm::vec3(0.f, 1.f, 0.f));
}

/////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Pickup> p;
//NEEDS A WAY TO DETERMINE PICKUP TYPES
void PickupManager::setupPickups(const std::shared_ptr<openGLHelper::Shader>& shader, std::vector<std::shared_ptr<render::Renderer::IRenderable>> &renderables) {

	std::shared_ptr<Pickup> pickup = std::make_shared<entity::Pickup>(shader);
	pickup->setPosition(onArenaPickupLocations.back());
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(pickup));
	p = pickup;
	//p->setPosition(onArenaPickupLocations.back());
	/*
	std::shared_ptr<Pickup> newPickup;
	for (int i = 0; i < numberOfPickups; i++) {
		newPickup = std::make_shared<Pickup>(shader);//USE STARTING POSITIONS AND TYPES IN CONSTRUCTOR
		allPickups.push_back(newPickup);
		activePickups.push_back(newPickup);
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////

void PickupManager::animatePickups(float fpsLimit) {
	p->animate(fpsLimit);
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
}	// namespace entity
}	// namespace hyperbright