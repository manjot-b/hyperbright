#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "Vehicle.h"
#define NOTARGET 0
#define HASTARGET 1
class Ai
{
public:
	Ai(std::shared_ptr<Vehicle> v);
	~Ai();
	void aiInput();
	int state;
	std::vector<glm::vec2> path;
	glm::vec2 targetTile;
	glm::vec2 targetTilePosition;
	std::shared_ptr<Vehicle> vehicle;
private:
	std::vector<glm::vec2> pathList;
	std::shared_ptr<Arena> arena;

	//Might change to indicate which direction to turn
	bool lookingAtTarget();//Retruns true if pointing in the direction of the targetTile
};
