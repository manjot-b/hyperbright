#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>
#include <vector>


#include "AudioPlayer.h"

#include "Arena.h"

#include "Model.h"
#include "Renderer.h"
#include "Simulate.h"
#include "Texture.h"
#include "Vehicle.h"

class Engine
{
public:
	Engine();
	~Engine();
	void run();
private:
	Camera camera;
	Renderer renderer;
	std::shared_ptr<Arena> arena;

	float deltaSec;
	//Ai aiPlayers[4];
	//Vehicle vehicles[4];
	//Pickup pickups[10];

	std::vector<std::shared_ptr<Vehicle>> vehicles;
	std::vector<std::shared_ptr<IPhysical>> physicsModels;
	std::vector<std::shared_ptr<Renderer::IRenderable>> renderables;

	// Rename/remove as required.
	std::shared_ptr<Texture> face;
	std::shared_ptr<Texture> tree;
	std::shared_ptr<Texture> background;

	std::shared_ptr<AudioPlayer> audioPlayer;
	void setupAudioPlayer();

	glm::vec3 rotate;
	float scale;

	float lastFrame;

	void runMenu();
	int menuInput();
	void runGame();
	void loadTextures();
	void initEntities();
};
