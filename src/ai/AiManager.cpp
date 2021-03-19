#include "AiManager.h"

namespace hyperbright {
namespace ai {
AiManager::AiManager() {}

AiManager::~AiManager() {}

//////////////////////////////////////////////////////////////////
void AiManager::loadAiVehicle(std::shared_ptr<entity::Vehicle> vehicle) {
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

void AiManager::setArena(std::shared_ptr<entity::Arena> a) {
	arena = a;
}

//////////////////////////////////////////////////////////////////
/*
void AiManager::setArena(std::vector<std::vector<bool>> a) {
	arenaRep = a;
}
*/

//////////////////////////////////////////////////////////////////
//GENERATE PATH FOR GIVEN AI
void AiManager::generatePath(std::shared_ptr<Ai> ai) {
	ai->targetTile = generateTarget(ai->targetTile);
	//std::cout << "Goal Tile for "<< ai->vehicle->getId()<<" : " << ai->targetTile.x << " "<< ai->targetTile.y << std::endl;
	//std::vector<glm::vec2> pathList;
	//glm::vec2 currentTile = ai->vehicle->currentTile;
	//glm::vec2 target = ai->targetTile;

	ai->path.push_back(ai->targetTile);
	ai->state = HASTARGET;
	return;

	/*

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
	*/
}

//////////////////////////////////////////////////////////////////
/*
a b c
d e f
g h i
*/
glm::vec2 a = glm::vec2(3,3);
glm::vec2 b = glm::vec2(20, 3);
glm::vec2 c = glm::vec2(36, 3);
glm::vec2 d = glm::vec2(3, 20);
glm::vec2 e = glm::vec2(20, 20);
glm::vec2 f = glm::vec2(36, 20);
glm::vec2 g = glm::vec2(3, 36);
glm::vec2 h = glm::vec2(20, 36);
glm::vec2 i = glm::vec2(36, 36);

//DECIDE WHERE TO GO NEXT
glm::vec2 AiManager::generateTarget(glm::vec2 lastGoal) {
	int selector;
	if (lastGoal == a) {
		selector = rand() % 2;
		if (selector == 0) {
			return b;
		}
		else {
			return d;
		}
	}
	else if (lastGoal == b) {
		selector = rand() % 3;
		if (selector == 0) {
			return a;
		}
		else if (selector == 1) {
			return c;
		}
		else {
			return e;
		}
	}
	else if (lastGoal == c) {
		selector = rand() % 2;
		if (selector == 0) {
			return b;
		}
		else  {
			return f;
		}
	}
	else if (lastGoal == d) {
		selector = rand() % 3;
		if (selector == 0) {
			return a;
		}
		else if (selector == 1) {
			return e;
		}
		else {
			return g;
		}
	}
	else if (lastGoal == e) {
		selector = rand() % 4;
		if (selector == 0) {
			return b;
		}
		else if (selector == 1) {
			return d;
		}
		else if (selector == 2) {
			return f;
		}
		else {
			return h;
		}
	}
	else if (lastGoal == f) {
		selector = rand() % 3;
		if (selector == 0) {
			return c;
		}
		else if (selector == 1) {
			return e;
		}
		else {
			return i;
		}
	}
	else if (lastGoal == g) {
		selector = rand() % 2;
		if (selector == 0) {
			return d;
		}
		else  {
			return h;
		}
	}
	else if (lastGoal == h) {
		selector = rand() % 3;
		if (selector == 0) {
			return g;
		}
		else if (selector == 1) {
			return e;
		}
		else {
			return i;
		}
	}
	else if (lastGoal == i) {
		selector = rand() % 2;
		if (selector == 0) {
			return h;
		}
		else  {
			return i;
		}
	}



	/*
	selector = rand() % 3;
	if (selector == 0) {
		return glm::vec2(20, 20);
	}
	else if(selector == 1){
		return glm::vec2(20, 30);
	}
	else if (selector == 2) {
		return glm::vec2(30, 30);
	}
	else if (selector == 3) {
		return glm::vec2(30, 20);
	} 
	return glm::vec2(50, 50);
	*/
}

//////////////////////////////////////////////////////////////////
//DECIDE NEXT TILE TO GO TO IN PATH (A*)
/*
bool AiManager::nextStep(glm::vec2 target, glm::vec2 currentTile, std::vector<glm::vec2> &pathList) {
	//std::cout << "Path Plan: "<< currentTile.x << " " << currentTile.y << std::endl;
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

*/
}	// namespace ai
}	// namespace hyperbright