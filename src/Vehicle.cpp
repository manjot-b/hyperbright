#include "Vehicle.h"

Vehicle::Vehicle() {
	pickupIndex = -1;
}

Vehicle::~Vehicle() {

}

void Vehicle::reset() {
	//Set to begining of game values
	pickupIndex = -1;
}