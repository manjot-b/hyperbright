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
		PickupManager::PickupManager(std::shared_ptr<entity::Arena> _arena, std::vector<std::shared_ptr<entity::Vehicle>>* _vehicles) :
			arena(_arena) {
			vehicles = _vehicles;
			pickupDownTime = 8.f;
		}

		PickupManager::~PickupManager() {
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::checkPickups() {
			checkCarriedPickups();
			checkActivePickups();
			checkTimerQueue();
			return;
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::initPickups(const std::shared_ptr<openGLHelper::Shader>& _shader) {
			shader = _shader;

			//NEEDED FOR ACTIVATE FUNCTION
			for (int i = 0; i < vehicles->size(); i++) {
				vehicles->at(i)->index = i;
			}

			onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(3, 3)) + glm::vec3(0.f, 1.f, 0.f));
			onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(36, 3)) + glm::vec3(0.f, 1.f, 0.f));
			onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(3, 36)) + glm::vec3(0.f, 1.f, 0.f));
			onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(36, 36)) + glm::vec3(0.f, 1.f, 0.f));
			onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(20, 20)) + glm::vec3(0.f, 1.f, 0.f));

			pickupIdCounter = 0;
			for (auto& pickupLocation : onArenaPickupLocations) {
				std::cout << pickupIdCounter << "\n";
				//std::shared_ptr<Pickup> pickup = std::make_shared<Pickup>(_i,0, std::shared_ptr<PickupManager>(this), shader);
				std::shared_ptr<Pickup> pickup = std::make_shared<Pickup>(pickupIdCounter, 0, nullptr, shader);
				pickup->setArenaLocation(pickupLocation , arena->isOnTile(pickupLocation));
				onArenaPickups.push_back(pickup);
				//std::cout << pickup->tile.x << " " << pickup->tile.y << "\n";
				toBeAddedPickups.push(pickup);
				pickupIdCounter++;
			}
		}

/////////////////////////////////////////////////////////////////////////////
		//Check timers on active pickups, deactivate those which are finished
		void PickupManager::checkCarriedPickups() {
			//check carried
			for (int i = 0; i < carriedPickups.size(); i++) {
				if (!carriedPickups.at(i)->beingCarried && carriedPickups.at(i)->active) {
					//std::cout << "PU ACTIVATED: " << carriedPickups.at(i)->pickupNumber <<"\n";
					carriedPickups.at(i)->activate();
					moveToActive(carriedPickups.at(i));
					removeFromCarried(carriedPickups.at(i));
				}
			}
		}

/////////////////////////////////////////////////////////////////////////////

		//Check timers on active pickups, deactivate those which are finished
		void PickupManager::checkActivePickups() {
			//check time
			for (int i = 0; i < activePickups.size(); i++) {
				if (!activePickups.at(i)->timeRemaining()) {
					std::shared_ptr<Pickup> pickup = activePickups.at(i);
					pickup->deactivate();
					removeFromActive(pickup);
					delete &pickup;
				}
			}
		}

/////////////////////////////////////////////////////////////////////////////

				//Check timers on active pickups, deactivate those which are finished
		void PickupManager::checkTimerQueue() {
			//check time
			if (pickupTimerQueue.size() > 0) {
				if (glfwGetTime() - pickupTimerQueue.front() > pickupDownTime) {

					std::shared_ptr<Pickup> pickup = std::make_shared<Pickup>(pickupIdCounter, 0, nullptr, shader);
					pickup->setArenaLocation(pickupPositionQueue.front(), arena->isOnTile(pickupPositionQueue.front()));
					onArenaPickups.push_back(pickup);
					toBeAddedPickups.push(pickup);
					pickupPositionQueue.pop();
					pickupTimerQueue.pop();
				}
			}
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::removeFromActive(std::shared_ptr<Pickup> pickup) {
			activePickups.erase(std::remove(activePickups.begin(), activePickups.end(), pickup));
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::removeFromArena(std::shared_ptr<Pickup> pickup) {
			onArenaPickups.erase(std::remove(onArenaPickups.begin(), onArenaPickups.end(), pickup));
			std::cout << onArenaPickups.size() << "\n";
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::removeFromCarried(std::shared_ptr<Pickup> pickup) {
			carriedPickups.erase(std::remove(carriedPickups.begin(), carriedPickups.end(), pickup));
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::moveToActive(std::shared_ptr<Pickup> pickup) {
			activePickups.push_back(pickup);
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::moveToInactive(std::shared_ptr<Pickup> pickup) {
			inactivePickups.push_back(pickup);
		}

void PickupManager::animate(float fps)
{
	for (auto& pickup : onArenaPickups) {
		pickup->animate(fps);
	}
}

void PickupManager::handlePickupOnCollision(glm::vec3 collisionPosition)
{
	std::cout << "Collision at: x " << collisionPosition.x << " y " << collisionPosition.y << " z " << collisionPosition.z << std::endl;
}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::moveToCarried(std::shared_ptr<Pickup> pickup) {
			carriedPickups.push_back(pickup);
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::moveToArena(std::shared_ptr<Pickup> pickup) {
			onArenaPickups.push_back(pickup);
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::animatePickups(float fps)
		{
			for (auto& pickup : onArenaPickups) {
				//std::cout<< pickup->tile.x << " " << pickup->tile.y << "\n";
				pickup->animate(fps);
			}
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::handlePickupOnCollision(Vehicle* v)
		{
			glm::vec2 curTile = v->currentTile;
			//std::cout << "COLLISION AT: \n" << curTile.x << " " << curTile.y<<"\n";
			for (int pickupIdCounter = 0; pickupIdCounter < onArenaPickups.size(); pickupIdCounter++) {
				if (onArenaPickups.at(pickupIdCounter)->tile.x == curTile.x && onArenaPickups.at(pickupIdCounter)->tile.y == curTile.y) {
					std::cout<< v->getId() <<" HAS PICKUP : " << onArenaPickups.at(pickupIdCounter)->pickupNumber << std::endl;

					pickupPositionQueue.push(arena->getTilePos(curTile) + glm::vec3(0.f, 1.f, 0.f));//ADD PICKUP LOCATION
					pickupTimerQueue.push(glfwGetTime());//START PICKUP RESPAWN TIMER
					onArenaPickups.at(pickupIdCounter)->beingCarried = true;
					v->pickupEquiped = onArenaPickups.at(pickupIdCounter);
					moveToCarried(onArenaPickups.at(pickupIdCounter));
					removeFromArena(onArenaPickups.at(pickupIdCounter));
					break;
				}
			}		
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::tearDown() {
			onArenaPickups.clear();
			inactivePickups.clear();
			activePickups.clear();
			for (int i = 0; i < allPickups.size(); i++) {
				delete& allPickups.end();
				allPickups.pop_back();
			}
		}
	}	// namespace entity
}	// namespace hyperbright