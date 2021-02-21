#include "AiManager.h"

AiManager::AiManager() {}

AiManager::~AiManager() {}

void AiManager::loadAiVehicle(std::shared_ptr<Vehicle> vehicle) {
	loadedAi.push_back(std::make_shared<Ai>(Ai(vehicle)));
}

void AiManager::makeMoves() {
	for (int i = 0; i < loadedAi.size(); i++) {
		loadedAi.at(i)->aiInput();
	}
}