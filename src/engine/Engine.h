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
#include "ui/HUD.h"
#include "render/IRenderable.h"
#include "render/Renderer.h"
#include "physics/Simulate.h"
#include "opengl-helper/Texture.h"
#include "opengl-helper/Shader.h"
#include "entity/Vehicle.h"
#include "entity/Pickup.h"
#include "entity/PickupManager.h"

namespace hyperbright {
namespace engine {
class Engine
{
public:
	Engine();
	~Engine();
	void run();
	
private:
	void buildArena1();
	void buildArena2();

	glm::vec2 playerStartingPosition;
	glm::vec2 ai1StartingPosition;
	glm::vec2 ai2StartingPosition;
	glm::vec2 ai3StartingPosition;

	render::Camera camera;
	ui::MainMenu mainMenu;
	ui::PauseMenu pauseMenu;
	ui::EndMenu endMenu;
	ui::DevUI devUI;
	std::unique_ptr<ui::HUD> playerHUD;
	std::shared_ptr<entity::Arena> arena;
	std::unique_ptr<Controller> controller;

	std::vector<std::shared_ptr<entity::Vehicle>> vehicles;
	std::vector<std::shared_ptr<physics::IPhysical>> physicsModels;
	std::vector<std::shared_ptr<render::IRenderable>> renderables;
	std::shared_ptr<openGLHelper::Shader> shader;

	std::shared_ptr<model::Model> battery;

	// Rename/remove as required.
	std::shared_ptr<openGLHelper::Texture> face;
	std::shared_ptr<openGLHelper::Texture> tree;
	std::shared_ptr<openGLHelper::Texture> background;

	std::shared_ptr<audio::AudioPlayer> audioPlayer;
	std::unique_ptr<entity::PickupManager> pickupManager;
	int currentArena;

	float fps;
	float deltaSec;
	float lastFrame;
	float roundTimer;	// seconds.

	void runMainMenu();
	void endGame();
	void runGame();
	void loadTextures();
	void initEntities();
	void initDevUI();
	void resetAll();

	void getDevUISettings();
};
}	// namespace engine
}	// namespace hyperbright