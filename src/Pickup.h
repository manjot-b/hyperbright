#pragma once
#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "PickupManager.h"
#include "Vehicle.h"

#define BATTERY 0
#define SPEED 1
#define ZAP 2
#define EMP 3
#define HIGHVOLTAGE 4
#define SLOWTRAP 5
#define SUCKER 6
#define SYPHON 7

class Pickup
{
public:
	Pickup();
	~Pickup();
	Pickup(int pickupType, PickupManager * pickupMan);
	void activate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace);
	void deactivate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace);
	void initialCollision(Vehicle vehicle);
	bool active;
	//bool beingCarried;
	bool timeRemaining();
	//Position
private:
	int type;
	float pickupTime;
	float pickUpStartTime;
	int zapOldColor;//CHANGE TYPE LATER
	bool slowTrapActive;
	float speedOldMax;
	Vehicle* carriedBy;
	PickupManager * pickupManager;
};
