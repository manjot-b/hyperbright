#include "Ai.h"

glm::vec2 targetTile;

Ai::Ai(std::shared_ptr<Vehicle> v) {
	vehicle = v;
	state = NOTARGET;
}

Ai::~Ai() {

}


//Determines if Ai choose a new goal or continue on current goal. Sets input accordingly.
void Ai::aiInput() {
	vehicle->accelerateForward();
	vehicle->turnHardRight();
}


