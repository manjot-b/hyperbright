#include "Pickup.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "PickupManager.h"
#include "Vehicle.h"
#include "engine/TeamStats.h"

namespace hyperbright {
	namespace entity {

		Pickup::Pickup() {
			pickupNumber = 0;
			setTriggerType(physics::IPhysical::TriggerType::PICKUP);
		}

		Pickup::Pickup(const std::shared_ptr<openGLHelper::Shader>& shader) : IRenderable(shader),
			model(std::make_shared<model::Model>("rsc/models/powerup.obj", "pickup", shader, nullptr)) {
			setTriggerType(physics::IPhysical::TriggerType::PICKUP);

			type = rand() % 5;//DEFAULT
			active = false;
			pickupNumber = 0;

			tile = glm::vec2(0, 0);
			//pickupNumber = puNum;
			//setArenaLocation(glm::vec3(0.f, 0.f, 3.f));
		}

		Pickup::~Pickup() {

		}

		/////////////////////////////////////////////////////////////////////

		Pickup::Pickup(int puNum, int pickupType, std::shared_ptr<PickupManager> pickupMan, const std::shared_ptr<openGLHelper::Shader>& shader) : Pickup(shader) {

			pickupManager = pickupMan;
			type = pickupType;
			active = false;
			beingCarried = false;
			slowTrapActive = false;
			pickupNumber = puNum;
			pickupTime = 5.f;
			//position

			model = std::make_shared<model::Model>("rsc/models/powerup.obj", "pickup", shader, nullptr);

		}

		/////////////////////////////////////////////////////////////////////

		void Pickup::activate(std::vector<std::shared_ptr<entity::Vehicle>>* _vehicles, std::shared_ptr<entity::Arena> arena) {
			//std::cout << "ACTIVATED:  ";
			pickUpStartTime = glfwGetTime();
			if (type == EMP) {
				std::cout << engine::teamStats::names.at(usedByTeam) << " USED EMP!! \n";
				_vehicles->at(0)->energy = 0;
				_vehicles->at(1)->energy = 0;
				_vehicles->at(2)->energy = 0;
				_vehicles->at(3)->energy = 0;
			}
			else if (type == ZAP) {
				//std::cout << "ZAP!! \n";

				engine::teamStats::Teams firstPlace;
				//engine::teamStats::Teams secondPlace = usedByTeam;

				engine::teamStats::Teams team0 = _vehicles->at(0)->getTeam();
				engine::teamStats::Teams team1 = _vehicles->at(1)->getTeam();
				engine::teamStats::Teams team2 = _vehicles->at(2)->getTeam();
				engine::teamStats::Teams team3 = _vehicles->at(3)->getTeam();
				/*
				//FIND FIRST PLACE

				if (engine::teamStats::scores.at(team0) >= engine::teamStats::scores.at(firstPlace)) {
					secondPlace = firstPlace;
					firstPlace = team0;
				}
				else if (engine::teamStats::scores.at(team1) >= engine::teamStats::scores.at(firstPlace)) {
					secondPlace = firstPlace;
					firstPlace = team1;
				}
				else if (engine::teamStats::scores.at(team2) >= engine::teamStats::scores.at(firstPlace)) {
					secondPlace = firstPlace;
					firstPlace = team2;
				}
				else if (engine::teamStats::scores.at(team3) >= engine::teamStats::scores.at(firstPlace)) {
					secondPlace = firstPlace;
					firstPlace = team3;
				}

				if (firstPlace == usedByTeam) {
					firstPlace = secondPlace;
				}
				*/

				firstPlace = _vehicles->at(rand() % 4)->getTeam();
				zapOldTeam = firstPlace;

				//zapOldTeam = firstPlace;

				std::cout << engine::teamStats::names.at(usedByTeam) << " ZAPPED " << engine::teamStats::names.at(zapOldTeam) << "\n";


				//CHANGE COLOR
				if (firstPlace == team0) {
					whoWasZapped = 0;
					_vehicles->at(0)->setTeam(usedByTeam);
				}
				else if (firstPlace == team1) {
					whoWasZapped = 1;
					_vehicles->at(1)->setTeam(usedByTeam);

				}
				else if (firstPlace == team2) {
					whoWasZapped = 2;
					_vehicles->at(2)->setTeam(usedByTeam);

				}
				else if (firstPlace == team3) {
					whoWasZapped = 3;
					_vehicles->at(3)->setTeam(usedByTeam);

				}

			}
			else if (type == SPEED) {//NEED IMPULSE FUNCTION
				std::cout << engine::teamStats::names.at(usedByTeam) << " USED SPEED!! \n";
				if (usedByTeam == _vehicles->at(0)->getTeam()) {
					//_vehicles->at(0)->
				}
				else if (usedByTeam == _vehicles->at(1)->getTeam()) {
					//_vehicles->at(1)->
				}
				else if (usedByTeam == _vehicles->at(2)->getTeam()) {
					//_vehicles->at(2)->
				}
				else if (usedByTeam == _vehicles->at(3)->getTeam()) {
					//_vehicles->at(3)->
				}

			}
			else if (type == SLOWTRAP) {
				std::cout << engine::teamStats::names.at(usedByTeam) << " LAYED A TRAP!! \n";
				//arena->insertSlowTrap();
			}
			else if (type == SYPHON) {
				std::cout << engine::teamStats::names.at(usedByTeam) << " USED SYPHON!! \n";
				if (usedByTeam == _vehicles->at(0)->getTeam()) {
					_vehicles->at(0)->syphonActive = true;
				}
				else if (usedByTeam == _vehicles->at(1)->getTeam()) {
					_vehicles->at(1)->syphonActive = true;
				}
				else if (usedByTeam == _vehicles->at(2)->getTeam()) {
					_vehicles->at(2)->syphonActive = true;
				}
				else if (usedByTeam == _vehicles->at(3)->getTeam()) {
					_vehicles->at(3)->syphonActive = true;
				}
			}
		}


		/////////////////////////////////////////////////////////////////////

		void Pickup::use(engine::teamStats::Teams team) {
			beingCarried = false;
			active = true;
			usedByTeam = team;
		}

		/////////////////////////////////////////////////////////////////////

		void Pickup::deactivate(std::vector<std::shared_ptr<entity::Vehicle>>* _vehicles) {
			if (type == ZAP) {
				if (whoWasZapped == 0) {
					_vehicles->at(0)->setTeam(zapOldTeam);
				}
				else if (whoWasZapped == 1) {
					_vehicles->at(1)->setTeam(zapOldTeam);
				}
				else if (whoWasZapped == 2) {
					_vehicles->at(2)->setTeam(zapOldTeam);
				}
				else if (whoWasZapped == 3) {
					_vehicles->at(3)->setTeam(zapOldTeam);
				}
			}
			else if (type == SYPHON) {
				if (usedByTeam == _vehicles->at(0)->getTeam()) {
					_vehicles->at(0)->syphonActive = false;
				}
				else if (usedByTeam == _vehicles->at(1)->getTeam()) {
					_vehicles->at(1)->syphonActive = false;
				}
				else if (usedByTeam == _vehicles->at(2)->getTeam()) {
					_vehicles->at(2)->syphonActive = false;
				}
				else if (usedByTeam == _vehicles->at(3)->getTeam()) {
					_vehicles->at(3)->syphonActive = false;
				}
			}
			std::cout << "DEACTIVATED\n";
		}

		/////////////////////////////////////////////////////////////////////

		bool Pickup::timeRemaining() {
			if (glfwGetTime() - pickUpStartTime > pickupTime) {
				return false;
			}
			return true;
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