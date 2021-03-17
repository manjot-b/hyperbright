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
	//void setupPickups(const std::shared_ptr<openGLHelper::Shader>& shader, std::vector<std::shared_ptr<render::Renderer::IRenderable>> &renderables);
	void checkPickups();
	void animatePickups(float fpsLimit);
	void tearDown();
	void initPickups(const std::shared_ptr<openGLHelper::Shader>& shader);

	std::vector< std::shared_ptr<Pickup> > carriedPickups;//pickups currently being carried by players

	std::vector< std::shared_ptr<Pickup> > onArenaPickups;//pickups to render and search for collisions
	std::queue< std::shared_ptr<Pickup> > toBeAddedPickups; // list of pickups to be added to arena this frame

	void removeFromCarried(std::shared_ptr<Pickup> pickup);
	void removeFromActive(std::shared_ptr<Pickup> pickup);
	void removeFromArena(std::shared_ptr<Pickup> pickup);
	void moveToInactive(std::shared_ptr<Pickup> pickup);
	void moveToActive(std::shared_ptr<Pickup> pickup);
	void moveToCarried(std::shared_ptr<Pickup> pickup);
	void moveToArena(std::shared_ptr<Pickup> pickup);
	void handlePickupOnCollision(Vehicle* v);
private: 
	int pickupIdCounter;
	void checkCarriedPickups();
	void checkActivePickups();
	void checkTimerQueue();
	float pickupDownTime;
	std::queue<float> pickupTimerQueue;
	std::queue< glm::vec3 > pickupPositionQueue;

	std::vector< std::shared_ptr<Pickup> > inactivePickups;//These pickups are not on the arena, and inactive
	std::vector< std::shared_ptr<Pickup> > activePickups;//These pickups are active, but not on the arena
	std::vector< std::shared_ptr<Pickup> > allPickups;//Keeps track of all the ptrs for easy tear down
	std::shared_ptr < entity::Arena > arena;

	std::shared_ptr<openGLHelper::Shader> shader;

	std::vector<glm::vec3> onArenaPickupLocations;
};
}	// namespace entity
}	// namespace hyperbright