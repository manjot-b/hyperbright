#pragma once
#include <vector>
#include "Pickup.h"
#include "Vehicle.h"

class PickupManager {
public:
	PickupManager();
	~PickupManager();
	void setupPickups(int numberOfPickups, std::vector <glm::vec3> startingPositions);
	void checkActivePickups();
	void tearDown();

	std::vector< std::shared_ptr<Pickup> > onArenaPickups;//pickups to render and search for collisions
	void removeFromActive(std::shared_ptr<Pickup> pickup);
	void removeFromArena(std::shared_ptr<Pickup> pickup);
	void moveToInactive(std::shared_ptr<Pickup> pickup);
	void moveToActive(std::shared_ptr<Pickup> pickup);
	void moveToArena(std::shared_ptr<Pickup> pickup);
private: 
	std::vector< std::shared_ptr<Pickup> > inactivePickups;//These pickups are not on the arena, and inactive
	std::vector< std::shared_ptr<Pickup> > activePickups;//These pickups are active, but not on the arena
	std::vector< std::shared_ptr<Pickup> > allPickups;//Keeps track of all the ptrs for easy tear down
};