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
	std::unique_ptr<Renderer> renderer;
	float deltaSec;

	Ai aiPlayers[4];
	Vehicle vehicles[4];
	Pickup pickups[10];

	std::vector<std::unique_ptr<Model>> models;
	std::vector<std::unique_ptr<Texture>> textures;
	std::vector<std::string> modelNames;
	std::shared_ptr<Camera> camera;

	glm::vec3 rotate;
	float scale;

	float lastFrame;

	void runMenu();
	int menuInput();
	void runGame();

	void loadModels(std::string ref);
	void loadTextures();
	void initEntities();
};
