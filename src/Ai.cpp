#include "Ai.h"

Ai::Ai(std::shared_ptr<Vehicle> v) {
	vehicle = v;
	state = NOTARGET;
	//path =  std::vector<glm::vec2>();
}

Ai::~Ai() {}


//Determines if Ai choose a new goal or continue on current goal. Sets input accordingly.
void Ai::aiInput() {

	if (vehicle->currentTile == targetTile) {
		std::cout << "GOAL REACHED"<< std::endl;
		path.pop_back();
		state = NOTARGET;
		return;
	}
	//std::cout << "Size:"<< path.size() << std::endl;
	//std::cout << "Current path:" << path.back().x << " " << path.back().y << std::endl;
	if (path.size() > 0) {
		if (vehicle->currentTile == path.back()) {
			std::cout << "NEXT TILE REACHED" << std::endl;
			path.pop_back();
		}

		float angleBetween = lookingAtTarget();
		if (angleBetween == 0) {
			vehicle->accelerateForward();
		}
		else if (angleBetween > 0) {
			vehicle->turnRight();
			vehicle->accelerateForward();
		}
		else {
			vehicle->turnLeft();
			vehicle->accelerateForward();
		}
	}
}

//Retruns the angle between the forward vector of the vehicle and a vector facing the next path tile.
//NEEDS A WAY TO INDICATE WHICH DIRECTION TO TURN
float Ai::lookingAtTarget() {
	return 0.f;
	glm::vec2 vehicleForwardVector = glm::vec2(vehicle->getForward().x, vehicle->getForward().z);
	glm::vec2 targetVector = glm::vec2(path.front().x - vehicle->getForward().x, path.front().y - vehicle->getForward().z);
	return acos(glm::dot(vehicleForwardVector, targetVector) / (glm::length(vehicleForwardVector) * glm::length(targetVector)));
}
