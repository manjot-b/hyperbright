#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>
#include <PxPhysicsAPI.h>

#include "Vehicle.h"
#include "Arena.h"
#include "Pickup.h"
#include "Model.h"

class Simulate
{
public:
	Simulate(std::vector<std::shared_ptr<Model>>& physicsModels);
	~Simulate();
	void stepPhysics();
	void setModelPose(std::shared_ptr<Model> &model);
	void cookMeshes();

	void cleanupPhysics();
private:
	void initPhysics();

	std::vector<std::shared_ptr<Model>> &physicsModels;
};
