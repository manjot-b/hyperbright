#pragma once
#include <glm/glm.hpp>

#include "Vehicle.h"
#include "Ai.h"
#include "Arena.h"

class AiManager
{
public:
	AiManager();
	~AiManager();
	AiManager(Arena a);
	void loadAiVehicle(std::shared_ptr<Vehicle> vehicle);
	void makeMoves();
	void setArena(std::vector<std::vector<bool>> arena);
	void setArena(std::shared_ptr<Arena> a);
private:
	std::vector<std::shared_ptr<Ai>> loadedAi;
	//std::vector<std::vector<bool>> arenaRep;
	std::shared_ptr<Arena> arena;

	void generatePath(std::shared_ptr<Ai> ai);
	glm::vec2 generateTarget();
	//bool nextStep(glm::vec2 target, glm::vec2 currentTile, std::vector<glm::vec2> &pathList);
};