#pragma once
#include <vector>
#include <queue>
#include "entity/Pickup.h"
#include "entity/Vehicle.h"


namespace hyperbright {
namespace entity {
class PickupManager {
public:
	PickupManager(std::shared_ptr<entity::Arena> arena);
	~PickupManager();
	void setupPickups(int numberOfPickups, std::vector <glm::vec3> startingPositions);
	void checkActivePickups();
	void tearDown();
	void initPickups();

	std::vector< std::shared_ptr<Pickup> > onArenaPickups;//pickups to render and search for collisions
	std::queue< std::shared_ptr<Pickup> > toBeAddedPickups; // list of pickups to be added to arena this frame
	void removeFromActive(std::shared_ptr<Pickup> pickup);
	void removeFromArena(std::shared_ptr<Pickup> pickup);
	void moveToInactive(std::shared_ptr<Pickup> pickup);
	void moveToActive(std::shared_ptr<Pickup> pickup);
	void moveToArena(std::shared_ptr<Pickup> pickup);
	void animate(float fps);
	void handlePickupOnCollision(glm::vec3 collisionPosition);
private: 
	std::vector< std::shared_ptr<Pickup> > inactivePickups;//These pickups are not on the arena, and inactive
	std::vector< std::shared_ptr<Pickup> > activePickups;//These pickups are active, but not on the arena
	std::vector< std::shared_ptr<Pickup> > allPickups;//Keeps track of all the ptrs for easy tear down
	std::shared_ptr < entity::Arena > arena;

	std::vector<glm::vec3> onArenaPickupLocations;
};
}	// namespace entity
}	// namespace hyperbright