#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <queue>

#include <memory>

#include "entity/Arena.h"
#include "entity/Pickup.h"
#include "model/Model.h"
#include "audio/AudioPlayer.h"

namespace hyperbright {
namespace entity {
	class Vehicle;
}	// namespace entity

namespace physics {
/*
Objects of this type are dynamic objects that should be part of the PhysX engine.
*/
class IPhysical
{
public:
	virtual void setModelMatrix(const glm::mat4& modelMat) = 0;
	virtual void setPosition(const glm::vec3& position) = 0;

	// TO-DO: Remove this method and have PhysX actors store pointers to whatever object
	// they need.
	virtual const char* getId() const = 0;
};

class Simulate
{
public:

	Simulate(std::vector<std::shared_ptr<IPhysical>>& physicsModels, 
		std::vector<std::shared_ptr<entity::Vehicle>>& vehicles, 
		const entity::Arena& arena,
		std::shared_ptr<entity::PickupManager>& pickupManager);
	~Simulate();
	void stepPhysics(float deltaSec);
	void setModelPose(std::shared_ptr<IPhysical>& model);
	void cookMeshes(const model::Model& mesh, void* _userData, bool useModelMatrix = false);
	void checkVehiclesOverTile(entity::Arena& arena, const std::vector<std::shared_ptr<entity::Vehicle>>& vehicles);
	void setAudioPlayer(std::shared_ptr<audio::AudioPlayer> audioPlayer);
	void cleanupPhysics();
private:
	void initPhysics();


	// physicsModels are all moving/colliding objects in the game including the vehicles
	// this list is used to update the graphical models with the transforms created by PhysX
	std::vector<std::shared_ptr<IPhysical>>& physicsModels;

	// A list of vehicles needed for initializing the vehicle actors and updating vehicle stats
	std::vector<std::shared_ptr<entity::Vehicle>>& vehicles;
};

void addPickup(std::shared_ptr<entity::Pickup>& pickup);
void removePickups();

namespace Driving {
void startAccelerateForwardsMode(int v);
void startAccelerateReverseMode(int v);
void startBrakeMode(int v);
void startTurnHardLeftMode(int v);
void startTurnHardRightMode(int v);
void startHandbrakeTurnLeftMode(int v);
void startHandbrakeTurnRightMode(int v);
void releaseAllControls(int v);
void applyVehicleFlipImpulse(int v);
}	// namespace Driving
}	// namespace physics
}	// namespace hyperbright