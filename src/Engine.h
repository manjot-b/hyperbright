#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Ai.h"
#include "Vehicle.h"

class Engine
{
public:
	Engine();
	~Engine();
	void run();
private:
	Ai aiPlayers[4];
	Vehicle vehicles[4];

	void runMenu();
	int menuInput();
	void runGame();
};
