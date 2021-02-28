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
	void setArena(std::shared_ptr<Arena> a);
private:
	std::vector<std::shared_ptr<Ai>> loadedAi;
	std::shared_ptr<Arena> arena;

	void generatePath(std::shared_ptr<Ai> ai);
};