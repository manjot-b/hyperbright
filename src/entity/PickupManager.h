#pragma once
#include <vector>
#include <queue>

#include "entity/Arena.h"
#include "entity/Pickup.h"
#include "entity/Vehicle.h"


namespace hyperbright {
namespace entity {
class PickupManager {
public:
	PickupManager(std::shared_ptr<entity::Arena> arena, std::vector<std::shared_ptr<entity::Vehicle>> * _vehicles, std::vector<std::shared_ptr<render::IRenderable>>& renderables);
	~PickupManager();

	void checkPickups();
	void animatePickups(float fpsLimit);
	void tearDown();
	void initPickups(const std::shared_ptr<openGLHelper::Shader>& shader, int arenaSelection);

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
	
	/* Finds the collided pickup via vehicle location, 
	*  removes that pickup from the renderables as well as
	*  and specific handling for that pickup,
	*  returns a pointer to that pickup
	*/
	std::shared_ptr<Pickup> handlePickupOnCollision(Vehicle* v);

private: 
	/* Container for pickup renderables.
	*  Responsible for holding the pointer to the pickup instance
	*  sent to the renderer as well as an identifying ID.
	*/
	struct PickupRenderables {
		PickupRenderables(std::shared_ptr<Pickup> pickup);
		std::shared_ptr<render::IRenderable> puRenderable;
		int pickupId;
	};
	std::vector<std::shared_ptr<entity::Vehicle>>* vehicles;

	int pickupIdCounter;
	void checkCarriedPickups();
	void checkActivePickups();
	void checkTimerQueue();
	void addPickupToScene(std::shared_ptr<Pickup>& pickup);
	float pickupDownTime;
	std::queue<float> pickupTimerQueue;
	std::queue< glm::vec3 > pickupPositionQueue;

	std::vector< std::shared_ptr<Pickup> > inactivePickups;//These pickups are not on the arena, and inactive
	std::vector< std::shared_ptr<Pickup> > activePickups;//These pickups are active, but not on the arena
	std::vector< std::shared_ptr<Pickup> > allPickups;//Keeps track of all the ptrs for easy tear down
	std::shared_ptr < entity::Arena > arena;
	std::shared_ptr<openGLHelper::Shader> shader;
	std::vector<glm::vec3> onArenaPickupLocations;

	/* List of pickups sent to the renderer.
	*  Seperate from any alternate utility lists.
	*/
	std::vector<std::shared_ptr<PickupRenderables>> renderedPickups;
	/* Reference to the games entire list of renderables.
	*  Used to find an remove pickups upon collision.
	*/
	std::vector<std::shared_ptr<render::IRenderable>>& renderables;
};
}	// namespace entity
}	// namespace hyperbright