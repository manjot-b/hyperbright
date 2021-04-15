#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <queue>

#include <memory>

#include "audio/AudioPlayer.h"
#include "entity/Arena.h"
#include "entity/ChargingStation.h"
#include "entity/Pickup.h"
#include "entity/PickupManager.h"
#include "entity/Vehicle.h"
#include "model/Model.h"
#include "physics/Interface.h"
#include "ui/DevUI.h"

namespace hyperbright {
namespace physics {

class Simulate
{
public:
	Simulate(std::vector<std::shared_ptr<IPhysical>>& physicsModels, 
		std::vector<std::shared_ptr<entity::Vehicle>>& vehicles, 
		const entity::Arena& arena,
		std::shared_ptr<entity::PickupManager>& pickupManager);
	~Simulate();
	
	void applyIntroForce(float maxSpeed);
	void stepPhysics(float deltaSec);
	void setModelPose(std::shared_ptr<IPhysical>& model);
	void cookMeshes(const model::Model& mesh, void* _userData, bool useModelMatrix = false);
	void checkVehiclesOverTile(entity::Arena& arena, const std::vector<std::shared_ptr<entity::Vehicle>>& vehicles);
	void setAudioPlayer(std::shared_ptr<audio::AudioPlayer> audioPlayer);
	void setConfigs(ui::DevUI::Settings::Handling turn);
	void cleanupPhysics();
private:
	struct FilterGroup
	{
		enum Enum
		{
			eVEHICLE = (1 << 0),
			eWALL = (1 << 1)
		};
	};

	void initPhysics();
	glm::vec2 arenaSize;

	// physicsModels are all moving/colliding objects in the game including the vehicles
	// this list is used to update the graphical models with the transforms created by PhysX
	std::vector<std::shared_ptr<IPhysical>>& physicsModels;

	// A list of vehicles needed for initializing the vehicle actors and updating vehicle stats
	std::vector<std::shared_ptr<entity::Vehicle>>& vehicles;
	void addChargingStations(const entity::Arena::ChargingStationList& stations);
}; //namespace Simulate

void addPickup(std::shared_ptr<entity::Pickup>& pickup);
void removePickups();

namespace Driving {
void startAccelerateForwardsMode(int v, float aDrive);
void startAccelerateReverseMode(int v, float aDrive);
void startBrakeMode(int v, float aDrive);
void startTurnHardLeftMode(int v, float aSteer);
void startTurnHardRightMode(int v, float aSteer);
void startHandbrakeTurnLeftMode(int v, float aSteer);
void startHandbrakeTurnRightMode(int v, float aSteer);
void releaseAllControls(int v);
void applyVehicleFlipImpulse(int v);
void applyVehicleBoost(int v);
void applyVehicleTrap(int v);
}	// namespace Driving
}	// namespace physics
}	// namespace hyperbright