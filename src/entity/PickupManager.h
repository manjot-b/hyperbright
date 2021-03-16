#pragma once
#include <vector>
#include "entity/Pickup.h"
#include "entity/Vehicle.h"


namespace hyperbright {
namespace entity {
class PickupManager {
public:
	PickupManager();
	~PickupManager();
	void setupPickups(const std::shared_ptr<openGLHelper::Shader>& shader, std::vector<std::shared_ptr<render::Renderer::IRenderable>> &renderables);
	void checkActivePickups();
	void animatePickups(float fpsLimit);
	void tearDown();
	void setUp(const std::shared_ptr<entity::Arena>& arena);
	std::vector<glm::vec3> onArenaPickupLocations;

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
}	// namespace entity
}	// namespace hyperbright