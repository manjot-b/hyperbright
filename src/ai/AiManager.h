#pragma once
#include <glm/glm.hpp>

#include "ai/Ai.h"
#include "entity/Vehicle.h"
#include "entity/Arena.h"
#include "ui/Menu.h"

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
	//void setArena(std::vector<std::vector<bool>> arena);
	void setArena(std::shared_ptr<entity::Arena> a, ui::MainMenu::ArenaSelection arenaSelection);
private:
	std::vector<std::shared_ptr<Ai>> loadedAi;
	//std::vector<std::vector<bool>> arenaRep;
	std::shared_ptr<entity::Arena> arena;
	ui::MainMenu::ArenaSelection currentArena;
	void generatePath(std::shared_ptr<Ai> ai);
	glm::vec2 generateTargetOne(glm::vec2 lastGoal, std::shared_ptr<Ai> ai);
	glm::vec2 generateTargetTwo(glm::vec2 lastGoal, std::shared_ptr<Ai> ai);
	//bool nextStep(glm::vec2 target, glm::vec2 currentTile, std::vector<glm::vec2> &pathList);

	glm::vec2 a;
	glm::vec2 b;
	glm::vec2 c;
	glm::vec2 d;
	glm::vec2 e;
	glm::vec2 f;
	glm::vec2 g;
	glm::vec2 h;
	glm::vec2 i;
	glm::vec2 j;
	glm::vec2 k;
	glm::vec2 l;
	glm::vec2 m;
	glm::vec2 n;
	glm::vec2 o;
	glm::vec2 p;
	glm::vec2 q;

};
}	// namespace ai
}	// namespace hyperbright