#include "Ai.h"

#include <GLFW/glfw3.h>

#include "engine/TeamStats.h"

namespace hyperbright {
	namespace ai {
		Ai::Ai(std::shared_ptr<entity::Vehicle> v, std::shared_ptr<entity::Arena> a , glm::vec2 startTile) {
			vehicle = v;
			arena = a;
			state = NOTARGET;
			targetTile = startTile;
			stuckCheck = false;
			stuckTimer = glfwGetTime();
			rollTimer = glfwGetTime();
			rollstuckTimeout = 4.f;
			stuckTimeout = 2.5f;
			backupTime = 1.5f;
			currentTile = startTile;
			pastGoal = glm::vec2(1, 1);//ARBITRARY
			pickupUseTime = (rand() % 10) + 5;
			//path =  std::vector<glm::vec2>();
		}

		Ai::~Ai() {}


		void Ai::updateCurrentTile() {
			if (currentTile != vehicle->currentTile) {
				currentTile = vehicle->currentTile;
				stuckTimer = glfwGetTime();
			}
		}



		//Determines if Ai choose a new goal or continue on current goal. Sets input accordingly.
		void Ai::aiInput() {
			updateCurrentTile();
			if (vehicle->currentTile == targetTile) {
				const std::string& name = engine::teamStats::names[vehicle->getTeam()];
				std::cout << name << " GOAL REACHED: " << targetTile.x << " " << targetTile .y << std::endl;
				path.pop_back();
				state = NOTARGET;
				return;
			}
			//Flip check
			if (!vehicle->isUpright() && glfwGetTime() - rollTimer > rollstuckTimeout) {
				vehicle->applyFlipImpulse();
				rollTimer = glfwGetTime();
			}
			//CRASH RECOVERY
			if (stuckCheck) {
				vehicle->stopForward();
				vehicle->turnLeft();
				vehicle->accelerateReverse();
				//std::cout << vehicle->getId() << " REVERSE!!\n";
				if (glfwGetTime() - backupTimerStart > backupTime) {
					stuckCheck = false;
					stuckTimer = glfwGetTime();
				}
				return;
			}

			if (glfwGetTime() - stuckTimer > stuckTimeout) {
				stuckCheck = true;
				backupTimerStart = glfwGetTime();
				//std::cout << vehicle->getId() << " STUCK!!\n";
				return;
			}

			//USE PICKUP CHECK
			if (vehicle->hasPickup()) {
				if (glfwGetTime() - pickupUseTime > vehicle->pickupTime) {
					std::cout << engine::teamStats::names[vehicle->getTeam()] << " USED PICKUP \n";
					vehicle->activatePickup();
				}
			}


			//std::cout << "Size:"<< path.size() << std::endl;
			//std::cout << "Current path:" << path.back().x << " " << path.back().y << std::endl;
			vehicle->stopReverse();
			if (path.size() > 0) {
				if (vehicle->currentTile == path.back()) {
					//std::cout << vehicle->getId() << " NEXT TILE REACHED " << vehicle->currentTile.x << " " << vehicle->currentTile.y << std::endl;
					path.pop_back();
				}
				//std::cout << vehicle->currentTile.x<< " "<< vehicle->currentTile.y << std::endl;
				//std::cout << targetTile.x << " " << targetTile.y << std::endl;
				float angleBetween = lookingAtTarget() * (180.f / M_PI);
				//float angleBetween = lookingAtTarget();

				vehicle->stopLeft();
				vehicle->stopRight();

				//std::cout <<angleBetween <<std::endl;
				if (angleBetween < 5.f && angleBetween > -5.f) {
					vehicle->accelerateForward();
					//std::cout << "straight" << std::endl;
				}
				else if (angleBetween > 0.f) {
					vehicle->turnRight();
					//std::cout << "right" << std::endl;
					vehicle->accelerateForward();
				}
				else {
					vehicle->turnLeft();
					//std::cout << "left" << std::endl;
					vehicle->accelerateForward();
				}
			}
		}

		//Retruns the angle between the forward vector of the vehicle and a vector facing the next path tile.
		//NEEDS A WAY TO INDICATE WHICH DIRECTION TO TURN
		float Ai::lookingAtTarget() {

			glm::vec3 targetPosition = arena->getTilePos(glm::vec2(path.front().x, path.front().y));
			//std::cout <<" target location: " <<targetPosition.x << " " << targetPosition.z << std::endl;

			glm::vec3 vehiclePosition = vehicle->getPosition();

			glm::vec2 vehicleDirectionVector = glm::normalize(glm::vec2(vehicle->getDirection().x, vehicle->getDirection().z));
			//std::cout << " Direction vector: " << vehicleDirectionVector.x << " " << vehicleDirectionVector.y << std::endl;

			glm::vec2 targetVector = glm::normalize(glm::vec2(targetPosition.x - vehiclePosition.x, targetPosition.z - vehiclePosition.z));
			//std::cout << " target vector: " << targetVector.x << " " << targetVector.y << std::endl;

			float dot = vehicleDirectionVector.x * -targetVector.y + vehicleDirectionVector.y * targetVector.x;
			if (dot > 0) {
				//std::cout << "b on the right of a" << std::endl;
				return -acos(glm::dot(vehicleDirectionVector, targetVector) / (glm::length(vehicleDirectionVector) * glm::length(targetVector)));
			}
			else if (dot < 0) {
				//std::cout << "b on the left of a" << std::endl;
				return acos(glm::dot(vehicleDirectionVector, targetVector) / (glm::length(vehicleDirectionVector) * glm::length(targetVector)));
			}

			return acos(glm::dot(vehicleDirectionVector, targetVector) / (glm::length(vehicleDirectionVector) * glm::length(targetVector)));
		}
	}	// namespace ai
}	// namespace hyperbright