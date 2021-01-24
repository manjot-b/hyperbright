#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>

#include "Ai.h"
#include "Vehicle.h"
#include "Pickup.h"
#include "Model.h"
#include "Renderer.h"

class Engine
{
public:
	Engine();
	~Engine();
	void run();
private:
	Renderer renderer;
	Ai aiPlayers[4];
	Vehicle vehicles[4];
	Pickup pickups[10];

	std::vector<std::unique_ptr<Model>> models;

	enum Scene
	{
		Test = 0,
		PhysX = 1,
	};

	float lastFrame;

	void runMenu();
	int menuInput();
	void runGame();
	void loadModels();
};
