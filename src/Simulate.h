#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <queue>

#include <memory>

#include "Arena.h"
#include "Pickup.h"
#include "Model.h"
#include "AudioPlayer.h"

class Vehicle;

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

	Simulate(std::vector<std::shared_ptr<IPhysical>>& physicsModels, std::vector<std::shared_ptr<Vehicle>>& vehicles, const Arena& arena);
	~Simulate();
	void stepPhysics(float deltaSec);
	void setModelPose(std::shared_ptr<IPhysical>& model);
	void cookMeshes(const std::shared_ptr<Model>& mesh, bool useModelMatrix = false);
	void checkVehiclesOverTile(Arena& arena, const std::vector<std::shared_ptr<Vehicle>>& vehicles);
	void setAudioPlayer(std::shared_ptr<AudioPlayer> audioPlayer);
	void cleanupPhysics();
private:
	void initPhysics();


	// physicsModels are all moving/colliding objects in the game including the vehicles
	// this list is used to update the graphical models with the transforms created by PhysX
	std::vector<std::shared_ptr<IPhysical>>& physicsModels;

	// A list of vehicles needed for initializing the vehicle actors and updating vehicle stats
	std::vector<std::shared_ptr<Vehicle>>& vehicles;

};

namespace Driving {
	void startAccelerateForwardsMode(int v);
	void startAccelerateReverseMode(int v);
	void startBrakeMode(int v);
	void startTurnHardLeftMode(int v);
	void startTurnHardRightMode(int v);
	void startHandbrakeTurnLeftMode(int v);
	void startHandbrakeTurnRightMode(int v);
	void releaseAllControls(int v);
}
