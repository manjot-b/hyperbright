#pragma once
#include <vector>
#include "Pickup.h"
#include "Vehicle.h"

class PickupManager {
public:
	PickupManager();
	~PickupManager();
	void setupPickups();
	void checkActivePickups();
	//std::vector<Pickup> onArenaPickups;//pickups to render and search for collisions
	//void moveToInactive(Pickup pickup);
private: 
	//std::vector<Pickup> inactivePickups;//These pickups are not on the arena, and inactive
	//std::vector<Pickup> activePickups;//These pickups are active, but not on the arena
};