#pragma once
#include <glm/glm.hpp>

#include "Vehicle.h"
#include "Ai.h"

class AiManager
{
public:
	AiManager();
	~AiManager();
	void loadAiVehicle(std::shared_ptr<Vehicle> vehicle);
	void makeMoves();
private:
	std::vector<std::shared_ptr<Ai>> loadedAi;
};