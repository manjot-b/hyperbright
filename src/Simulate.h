#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <queue>

#include <memory>

#include "Vehicle.h"
#include "Arena.h"
#include "Pickup.h"
#include "Model.h"

class Simulate
{
public:
	Simulate(std::vector<std::shared_ptr<Model>>& physicsModels);
	~Simulate();
	void stepPhysics(bool input[], float deltaSec);
	void setModelPose(std::shared_ptr<Model>& model);
	void cookMeshes();
	void checkVehicleOverTile(Arena& arena, Model& model);

	void cleanupPhysics();
private:
	void initPhysics();

	std::vector<std::shared_ptr<Model>>& physicsModels;
};
