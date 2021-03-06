#include "AiManager.h"

AiManager::AiManager() {}

AiManager::~AiManager() {}

//////////////////////////////////////////////////////////////////
int counter;
void AiManager::loadAiVehicle(std::shared_ptr<Vehicle> vehicle) {
	loadedAi.push_back(std::make_shared<Ai>(Ai(vehicle, arena) ));
}

//////////////////////////////////////////////////////////////////

void AiManager::makeMoves() {
	for (int i = 0; i < loadedAi.size(); i++) {
		if (loadedAi.at(i)->state) {
			loadedAi.at(i)->aiInput();
		}
		else {
			if (loadedAi.at(i)->vehicle->currentTile.x != -1) {
				generatePath(loadedAi.at(i));
				//std::cout<<loadedAi.at(i)->path.size()<<std::endl;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////

void AiManager::setArena(std::shared_ptr<Arena> a) {
	arena = a;
	counter = 0;
}

//////////////////////////////////////////////////////////////////

void AiManager::setArena(std::vector<std::vector<bool>> a) {
	arenaRep = a;
}

//////////////////////////////////////////////////////////////////
//GENERATE PATH FOR GIVEN AI
void AiManager::generatePath(std::shared_ptr<Ai> ai) {
	ai->targetTile = generateTarget();
	std::cout << "counter: " << counter << std::endl;
	std::vector<glm::vec2> pathList;
	glm::vec2 currentTile = ai->vehicle->currentTile;
	glm::vec2 target = ai->targetTile;
	//std::shared_ptr<std::vector<glm::vec2>> pathListPtr = std::make_shared<std::vector<glm::vec2>>(pathList);


	//std::cout << currentTile.x << " " << currentTile.y << std::endl;
	if (currentTile.x < 0 || currentTile.y < 0) {//NEED FOR INITAL COLLISION COMING UP AS NEGATIVE
		return;
	}

	if (target.x != currentTile.x) {//CHECK X AXIS
		if (target.x > currentTile.x && currentTile.x + 1 < arenaRep.size() && nextStep(target, glm::vec2(currentTile.x + 1, currentTile.y), ai->path)) {//RIGHT
			//ai->path = pathList;
			ai->state = HASTARGET;
			return;
		}
		else if (currentTile.x - 1 >= 0 && nextStep(target, glm::vec2(currentTile.x - 1, currentTile.y), ai->path)) {//LEFT
			//ai->path = pathList;
			ai->state = HASTARGET;
			return;
		}
		//FORCE ALTERNATIVE / WALL PROTOCOL
	}

	//CHECK Y AXIS
	if (target.y > currentTile.y && currentTile.y + 1 < arenaRep.at(0).size() && nextStep(target, glm::vec2(currentTile.x, currentTile.y + 1), ai->path)) {//DOWN

		//ai->path = pathList;
		ai->state = HASTARGET;
		return;

	}
	else if (currentTile.y - 1 >= 0 && nextStep(target, glm::vec2(currentTile.x, currentTile.y - 1), ai->path)) {//UP

		//ai->path = pathList;
		ai->state = HASTARGET;
		return;

	}
	//FORCE ALTERNATIVE / WALL PROTOCOL
}

//////////////////////////////////////////////////////////////////
//DECIDE WHERE TO GO NEXT

glm::vec2 AiManager::generateTarget() {
	return glm::vec2(20, 41);
}

//////////////////////////////////////////////////////////////////
//DECIDE NEXT TILE TO GO TO IN PATH
bool AiManager::nextStep(glm::vec2 target, glm::vec2 currentTile, std::vector<glm::vec2> &pathList) {
	std::cout << "Path Plan: "<< currentTile.x << " " << currentTile.y << std::endl;
	if (arenaRep[currentTile.x][currentTile.y]) {//BASE CASE 1 WALL
		return false;
	}
	
	if (target == currentTile) {//BASE CASE 2 TARGET FOUND
		pathList.push_back(target);
		return true;
	}


	if (target.x!=currentTile.x) {//CHECK X AXIS
		if (target.x > currentTile.x && currentTile.x + 1 < arenaRep.size() && nextStep(target, glm::vec2(currentTile.x + 1, currentTile.y), pathList)) {//RIGHT
			pathList.push_back(currentTile);
			return true;
		}
		else if(currentTile.x - 1 >= 0 && nextStep(target, glm::vec2(currentTile.x - 1, currentTile.y), pathList)){//LEFT
			pathList.push_back(currentTile);
			return true;
		}
		//FORCE ALTERNATIVE / WALL PROTOCOL
	}

	//CHECK Y AXIS
	if (target.y > currentTile.y && currentTile.y + 1 < arenaRep.at(0).size() && nextStep(target, glm::vec2(currentTile.x, currentTile.y + 1), pathList)) {//DOWN

		pathList.push_back(currentTile);
		return true;

	}else if (currentTile.y - 1 >= 0 && nextStep(target, glm::vec2(currentTile.x, currentTile.y - 1), pathList)) {//UP

		pathList.push_back(currentTile);
		return true;

	}
	//FORCE ALTERNATIVE / WALL PROTOCOL
	
	return false;
}