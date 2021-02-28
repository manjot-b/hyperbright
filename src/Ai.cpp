#include "Ai.h"

glm::vec2 targetTile;
glm::vec2 targetTilePosition;


Ai::Ai(std::shared_ptr<Vehicle> v) {
	vehicle = v;
	state = NOTARGET;
}

Ai::~Ai() {

}


//Determines if Ai choose a new goal or continue on current goal. Sets input accordingly.
void Ai::aiInput() {
	if (!lookingAtTarget()) {
		vehicle->turnRight();
		vehicle->accelerateForward();
	} else {
		vehicle->accelerateForward();
	}
}

//Retruns true if pointing in the direction of the targetTile
//NEEDS A WAY TO INDICATE WHICH DIRECTION TO TURN
bool Ai::lookingAtTarget() {
	return false;
}
