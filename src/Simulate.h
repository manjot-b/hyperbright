#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <queue>

#include <memory>

#include "Vehicle.h"
#include "Arena.h"
#include "Pickup.h"
#include "Model.h"

using namespace std;

class Simulate
{
public:
	Simulate(vector<shared_ptr<Model>>& physicsModels, vector<shared_ptr<Vehicle>>& vehicles);
	~Simulate();
	void stepPhysics(bool input[], float deltaSec);
	void setModelPose(std::shared_ptr<Model>& model);
	void cookMeshes();
	void checkVehicleOverTile(Arena& arena, Model& model);

	void cleanupPhysics();
private:
	void initPhysics();

	// physicsModels are all moving/colliding objects in the game including the vehicles
	// this list is used to update the graphical models with the transforms created by PhysX
	vector<shared_ptr<Model>>& physicsModels;

	// A list of vehicles needed for initializing the vehicle actors and updating vehicle stats
	vector<shared_ptr<Vehicle>>& vehicles;
};
