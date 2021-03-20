#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "entity/Vehicle.h"

#define NOTARGET 0
#define HASTARGET 1

namespace hyperbright {
namespace ai {
class Ai
{
public:
	Ai(std::shared_ptr<entity::Vehicle> v, std::shared_ptr<entity::Arena> a);
	~Ai();
	void aiInput();
	int state;
	std::vector<glm::vec2> path;
	glm::vec2 currentTile;
	glm::vec2 targetTile;
	glm::vec2 targetTilePosition;
	glm::vec2 pastGoal;
	std::shared_ptr<entity::Vehicle> vehicle;
	std::shared_ptr<entity::Arena> arena;
private:
	float stuckTimer;
	bool stuckCheck;
	float stuckTimeout;
	float backupTimerStart;
	float backupTime;
	//Might change to indicate which direction to turn
	float lookingAtTarget();//Retruns true if pointing in the direction of the targetTile
	void updateCurrentTile();
};
}	// namespace ai
}	// namespace hyperbright