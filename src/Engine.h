#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>

#include "Simulate.h"
#include "Arena.h"
#include "Pickup.h"
#include "Ai.h"
#include "Vehicle.h"
#include "DevUI.h"
#include "Controller.h"
#include "Renderer.h"
#include "PickupManager.h"
#include "Texture.h"

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

	std::vector<std::shared_ptr<Model>> staticModels;
	std::vector<std::shared_ptr<Model>> physicsModels;
	std::shared_ptr<Camera> camera;

	// These should eventually be their specific classes rather than Model.
	// e.g. the plane should be Arena, car should be Vehicle.
	std::shared_ptr<Model> grid;
	std::shared_ptr<Model> vehicle;
	std::shared_ptr<Model> skyBox;

	// Rename/remove as required.
	std::shared_ptr<Texture> face;
	std::shared_ptr<Texture> tree;
	std::shared_ptr<Texture> background;

	glm::vec3 rotate;
	float scale;

	float lastFrame;

	void runMenu();
	int menuInput();
	void runGame();
  
	std::shared_ptr<Model> loadModel(std::string ref, bool inPhysx, Model::MoveType type, const std::shared_ptr<Texture>& texture);
	void loadTextures();
	void initEntities();
};
