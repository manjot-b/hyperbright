#include "PickupManager.h"

#include "Pickup.h"
#include "Vehicle.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

namespace hyperbright {
	namespace entity {
		PickupManager::PickupManager(std::shared_ptr<audio::AudioPlayer>& _audioPlayer, std::shared_ptr<entity::Arena> _arena, std::vector<std::shared_ptr<entity::Vehicle>>* _vehicles, std::vector<std::shared_ptr<render::IRenderable>>& _renderables) :
			arena(_arena), renderables(_renderables) , audioPlayer(_audioPlayer){
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

		void PickupManager::initPickups(const std::shared_ptr<openGLHelper::Shader>& _shader, ui::MainMenu::ArenaSelection arenaSelection) {
			shader = _shader;

			//NEEDED FOR ACTIVATE FUNCTION
			for (int i = 0; i < vehicles->size(); i++) {
				vehicles->at(i)->index = i;
			}

			if (arenaSelection == ui::MainMenu::ArenaSelection::ARENA1) {
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(3, 3)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(36, 3)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(3, 36)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(36, 36)) + glm::vec3(0.f, 1.f, 0.f));
				//onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(20, 20)) + glm::vec3(0.f, 1.f, 0.f));
			}
			else if (arenaSelection == ui::MainMenu::ArenaSelection::ARENA2) {
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(2, 2)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(17, 2)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(2, 17)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(17, 17)) + glm::vec3(0.f, 1.f, 0.f));
			}
			else if (arenaSelection == ui::MainMenu::ArenaSelection::ARENA3) {
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(14, 26)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(2, 3)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(26, 3)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(9, 15)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(20, 15)) + glm::vec3(0.f, 1.f, 0.f));
			}
			else if (arenaSelection == ui::MainMenu::ArenaSelection::ARENA4) {
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(8, 8)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(8, 6)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(6, 8)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(6, 6)) + glm::vec3(0.f, 1.f, 0.f));

				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(7, 6)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(7, 8)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(6, 7)) + glm::vec3(0.f, 1.f, 0.f));
				onArenaPickupLocations.push_back(arena->getTilePos(glm::vec2(8, 7)) + glm::vec3(0.f, 1.f, 0.f));
			}

			pickupIdCounter = 1;
			for (auto& pickupLocation : onArenaPickupLocations) {
				std::cout << "Pickup " << pickupIdCounter << " initialized.\n";
				std::shared_ptr<Pickup> pickup = std::make_shared<Pickup>(pickupIdCounter, rand()%5, nullptr, shader);
				pickup->setArenaLocation(pickupLocation , arena->isOnTile(pickupLocation));
				addPickupToScene(pickup);	// encapsulated new pickup calls
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
					if (carriedPickups.at(i)->type == EMP) {
						audioPlayer->playEmpSound();
					}
					carriedPickups.at(i)->activate(vehicles);
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
					pickup->deactivate(vehicles, arena);
					removeFromActive(pickup);
					//delete pickup;
				}
			}
		}

/////////////////////////////////////////////////////////////////////////////

		//Check timers on active pickups, deactivate those which are finished
		void PickupManager::checkTimerQueue() {
			//check time
			if (pickupTimerQueue.size() > 0) {
				if (glfwGetTime() - pickupTimerQueue.front() > pickupDownTime) {
					//////////////// Too tired to think of a better way to find the missing pickup id
					int newPickupId = 1;
					for (int i = 0; i < renderedPickups.size(); i++) {
						bool idFound = false;
						for (auto& pickup : renderedPickups) {
							if (pickup->pickupId == newPickupId) {
								idFound = true;
								newPickupId++;
								break;
							}
						}
						if (!idFound) break;
					}
					////////////////
					std::shared_ptr<Pickup> pickup = std::make_shared<Pickup>(newPickupId, rand() % 5, nullptr, shader);		// This constructor was using an invalid pickupId
					pickup->setArenaLocation(pickupPositionQueue.front(), arena->isOnTile(pickupPositionQueue.front()));
					addPickupToScene(pickup);	// encapsulated new pickup calls
					pickupPositionQueue.pop();
					pickupTimerQueue.pop();
				}
			}
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::addPickupToScene(std::shared_ptr<Pickup>& pickup) {
			onArenaPickups.push_back(pickup);	// add to list of all pickups
			toBeAddedPickups.push(pickup);		// add to list of physx actors needed
			renderedPickups.push_back(std::make_shared<PickupRenderables>(pickup));	// add to list of rendered pickups
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::removeFromActive(std::shared_ptr<Pickup> pickup) {
			activePickups.erase(std::remove(activePickups.begin(), activePickups.end(), pickup));
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::removeFromArena(std::shared_ptr<Pickup> pickup) {
			// searches the list of pickups that should be render and finds the PickupRenderables 
			// that matches given pickup.  Then searches the list of all renderable objects and
			// finds the IRenderable that matches the PickupRenerables. 
			// Finally, deletes the pickup references from both lists.
			std::vector<std::shared_ptr<PickupRenderables>>::iterator it_pum;
			for (it_pum = renderedPickups.begin(); it_pum != renderedPickups.end(); it_pum++) {
				if (it_pum->get()->pickupId == pickup->pickupNumber) {
					std::vector<std::shared_ptr<render::IRenderable>>::iterator it_ren;
					for (it_ren = renderables.begin(); it_ren != renderables.end(); it_ren++) {
						if (*it_pum->get()->puRenderable == it_ren->get()) {
							renderables.erase(it_ren);
							renderedPickups.erase(it_pum);
							break;
						}
					}
					break;
				}
			}

			onArenaPickups.erase(std::remove(onArenaPickups.begin(), onArenaPickups.end(), pickup));
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

		std::shared_ptr<Pickup> PickupManager::handlePickupOnCollision(Vehicle* v)
		{
			glm::vec2 curTile = v->currentTile;
			//std::cout << "COLLISION AT: \n" << curTile.x << " " << curTile.y<<"\n";
			for (int IdCounter = 0; IdCounter < onArenaPickups.size(); IdCounter++) {
				std::shared_ptr<Pickup> pu = onArenaPickups.at(IdCounter);
				if (pu->tile.x == curTile.x && pu->tile.y == curTile.y) {
					const std::string& name = engine::teamStats::names[v->getTeam()];
					//std::cout<< name <<" HIT PICKUP : " << pu->pickupNumber << std::endl;

					pickupPositionQueue.push(arena->getTilePos(curTile) + glm::vec3(0.f, 1.f, 0.f));//ADD PICKUP LOCATION
					pickupTimerQueue.push(glfwGetTime());//START PICKUP RESPAWN TIMER

					if (v->hasPickup()) {
						std::cout << name << " ALREADY HAS PICKUP: " << v->getPickup()->pickupNumber <<std::endl;
						removeFromArena(pu);
						return pu;
					}

					pu->beingCarried = true;
					v->equipPickup(pu);
					v->pickupTime = glfwGetTime();

					moveToCarried(pu);
					removeFromArena(pu);
					return pu;
				}
			}	
			// ! ! ! Do not comment out this error msg ! ! !
			std::cout << "! ! ! ERROR: Pickup not found ! ! !" << std::endl;
			std::shared_ptr<Pickup> pu = std::make_shared<Pickup>();	// if pickup not found, return unused pickup
			return pu;
		}

/////////////////////////////////////////////////////////////////////////////

		void PickupManager::tearDown() {
			onArenaPickups.clear();
			inactivePickups.clear();
			activePickups.clear();
			allPickups.clear();
		}

		PickupManager::PickupRenderables::PickupRenderables(std::shared_ptr<Pickup> pickup) : puRenderable(pickup), pickupId(pickup->pickupNumber) {}
}	// namespace entity
}	// namespace hyperbright
