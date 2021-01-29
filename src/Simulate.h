#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Vehicle.h"
#include "Arena.h"
#include "Pickup.h"
#include "Model.h"

class Simulate
{
public:
	Simulate();
	~Simulate();
	void stepPhysics();
	void setModelPose(std::unique_ptr<Model> &model);
	void cleanupPhysics();
private:
	void initPhysics();
};
