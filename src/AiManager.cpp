#include "AiManager.h"

AiManager::AiManager() {}

AiManager::~AiManager() {}

void AiManager::loadAiVehicle(std::shared_ptr<Vehicle> vehicle) {
	loadedAi.push_back(std::make_shared<Ai>(Ai(vehicle)));
}

void AiManager::makeMoves() {
	for (int i = 0; i < loadedAi.size(); i++) {
		if (loadedAi.at(i)->state) {
			loadedAi.at(i)->aiInput();
		}
		else {
			generatePath(loadedAi.at(i));
		}
	}
}

void AiManager::setArena(std::shared_ptr<Arena> a) {
	arena = a;
}

void AiManager::generatePath(std::shared_ptr<Ai> ai) {


	ai->state = HASTARGET;
}