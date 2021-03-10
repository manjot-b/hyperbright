#pragma once

#include <string>

#include <glm/glm.hpp>

#include <memory>

#define BATTERY 0
#define SPEED 1
#define ZAP 2
#define EMP 3
#define HIGHVOLTAGE 4
#define SLOWTRAP 5
#define SUCKER 6
#define SYPHON 7

namespace hyperbright {
namespace entity {
	class Vehicle;//Forward declaration
	class PickupManager;
}	// namespace entity

namespace entity {
class Pickup
{
public:
	Pickup();
	~Pickup();
	Pickup(int pickupType, std::shared_ptr<PickupManager> pickupManager );
	void activate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace);
	void deactivate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace);
	void initialCollision(std::shared_ptr<Vehicle> vehicle);
	bool active;
	//bool beingCarried;
	bool timeRemaining();
	//Position
private:
	int type;
	float pickupTime;
	float pickUpStartTime;
	glm::vec4 zapOldColor;//CHANGE TYPE LATER
	bool slowTrapActive;
	float speedOldMax;
	//Vehicle* carriedBy;
	std::shared_ptr<PickupManager> pickupManager;
};
}	// namespace entity
}	// namespace hyperbright