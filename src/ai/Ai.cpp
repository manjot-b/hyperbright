#include "Ai.h"
Ai::Ai(std::shared_ptr<Vehicle> v, std::shared_ptr<Arena> a) {
	vehicle = v;
	arena = a;
	state = NOTARGET;
	//path =  std::vector<glm::vec2>();
}

Ai::~Ai() {}


//Determines if Ai choose a new goal or continue on current goal. Sets input accordingly.
void Ai::aiInput() {

	if (vehicle->currentTile == targetTile) {
		std::cout << vehicle->getId() << " GOAL REACHED"<< std::endl;
		path.pop_back();
		state = NOTARGET;
		return;
	}
	//std::cout << "Size:"<< path.size() << std::endl;
	//std::cout << "Current path:" << path.back().x << " " << path.back().y << std::endl;
	if (path.size() > 0) {
		if (vehicle->currentTile == path.back()) {
			std::cout<<vehicle->getId() << " NEXT TILE REACHED " << vehicle->currentTile.x<<" "<< vehicle->currentTile.y << std::endl;
			path.pop_back();
		}
		//std::cout << vehicle->currentTile.x<< " "<< vehicle->currentTile.y << std::endl;
		//std::cout << targetTile.x << " " << targetTile.y << std::endl;
		float angleBetween = lookingAtTarget() * (180.f/M_PI);
		//float angleBetween = lookingAtTarget();

		vehicle->stopLeft();
		vehicle->stopRight();

		//std::cout <<angleBetween <<std::endl;
		if (angleBetween < 5.f && angleBetween > -5.f) {

			vehicle->accelerateForward();
			//std::cout << "straight" << std::endl;
		}
		else if (angleBetween > 0.f ) {
			//vehicle->stopLeft();
			vehicle->turnRight();
			//std::cout << "right" << std::endl;
			vehicle->accelerateForward();
		}
		else {
			//vehicle->stopRight();
			vehicle->turnLeft();
			//std::cout << "left" << std::endl;
			vehicle->accelerateForward();
		}
	}
}

//Retruns the angle between the forward vector of the vehicle and a vector facing the next path tile.
//NEEDS A WAY TO INDICATE WHICH DIRECTION TO TURN
float Ai::lookingAtTarget() {
	//return 0.f;

	glm::vec3 targetPosition = arena->getTilePos(glm::vec2(path.front().x, path.front().y));
	//std::cout <<" target location: " <<targetPosition.x << " " << targetPosition.z << std::endl;

	glm::vec3 vehiclePosition = vehicle->getPosition();


	glm::vec2 vehicleDirectionVector = glm::normalize(glm::vec2(vehicle->getDirection().x, vehicle->getDirection().z));
	//std::cout << " Direction vector: " << vehicleDirectionVector.x << " " << vehicleDirectionVector.y << std::endl;

	//glm::vec2 targetVector = glm::vec2( vehiclePosition.x - targetPosition.x , vehiclePosition.z - targetPosition.z);
	glm::vec2 targetVector = glm::normalize(glm::vec2( targetPosition.x - vehiclePosition.x, targetPosition.z - vehiclePosition.z));
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
