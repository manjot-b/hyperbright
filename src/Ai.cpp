#include "Ai.h"


Ai::Ai(std::shared_ptr<Vehicle> v) {
	vehicle = v;
}

Ai::~Ai() {

}

//Determines if Ai choose a new goal or continue on current goal. Sets input accordingly.
void Ai::aiInput() {

	vehicle->accelerateForward();
	vehicle->turnHardRight();
}