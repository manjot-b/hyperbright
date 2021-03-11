#pragma once
#include <glm/glm.hpp>

#include "ai/Ai.h"
#include "entity/Vehicle.h"
#include "entity/Arena.h"

namespace hyperbright {
namespace ai {
class AiManager
{
public:
	AiManager();
	~AiManager();
	AiManager(entity::Arena a);
	void loadAiVehicle(std::shared_ptr<entity::Vehicle> vehicle);
	void makeMoves();
	void setArena(std::vector<std::vector<bool>> arena);
	void setArena(std::shared_ptr<entity::Arena> a);
private:
	std::vector<std::shared_ptr<Ai>> loadedAi;
	//std::vector<std::vector<bool>> arenaRep;
	std::shared_ptr<entity::Arena> arena;

	void generatePath(std::shared_ptr<Ai> ai);
	glm::vec2 generateTarget(glm::vec2 lastGoal);
	//bool nextStep(glm::vec2 target, glm::vec2 currentTile, std::vector<glm::vec2> &pathList);
};
}	// namespace ai
}	// namespace hyperbright