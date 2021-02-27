#pragma once

#include <glm/glm.hpp>

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
private:
	std::vector<glm::vec2> pathList;
	std::shared_ptr<Vehicle> vehicle;
	std::shared_ptr<Arena> arena;
};
