#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Ai.h"
#include "Vehicle.h"
#include "Pickup.h"

class Engine
{
public:
	Engine();
	~Engine();
	void run();
private:
	Ai aiPlayers[4];
	Vehicle vehicles[4];
	Pickup pickups[10];

	enum Scene
	{
		Test = 0,
		PhysX = 1,
	};

	float deltaTime;
	float lastFrame;

	void runMenu();
	int menuInput();
	void runGame();
};
