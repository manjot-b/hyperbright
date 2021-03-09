#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>
#include <vector>

#include "audio/AudioPlayer.h"
#include "entity/Arena.h"
#include "Controller.h"
#include "model/Model.h"
#include "ui/Menu.h"
#include "ui/DevUI.h"
#include "render/Renderer.h"
#include "physics/Simulate.h"
#include "opengl-helper/Texture.h"
#include "entity/Vehicle.h"

class Engine
{
public:
	Engine();
	~Engine();
	void run();
private:
	Camera camera;
	Menu menu;
	DevUI devUI;
	std::shared_ptr<Arena> arena;
	std::unique_ptr<Controller> controller;

	std::vector<std::shared_ptr<Vehicle>> vehicles;
	std::vector<std::shared_ptr<IPhysical>> physicsModels;
	std::vector<std::shared_ptr<Renderer::IRenderable>> renderables;

	std::shared_ptr<Model> battery;

	// Rename/remove as required.
	std::shared_ptr<Texture> face;
	std::shared_ptr<Texture> tree;
	std::shared_ptr<Texture> background;

	std::shared_ptr<AudioPlayer> audioPlayer;

	float deltaSec;
	float lastFrame;
	float roundTimer;	// seconds.

	void runMainMenu();
	void endGame();
	void runGame();
	void loadTextures();
	void initEntities();
	void setupAudioPlayer();
};
