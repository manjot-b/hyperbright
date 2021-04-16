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
#include "entity/Vehicle.h"
#include "entity/Pickup.h"
#include "entity/PickupManager.h"
#include "Controller.h"
#include "model/Model.h"
#include "ui/Menu.h"
#include "ui/DevUI.h"
#include "ui/HUD.h"
#include "ui/LoadingScreen.h"
#include "render/IRenderable.h"
#include "render/Renderer.h"
#include "physics/Simulate.h"
#include "opengl-helper/Texture.h"
#include "opengl-helper/Shader.h"


namespace hyperbright {
namespace engine {
class Engine
{
public:
	Engine();
	~Engine();
	void run();
	
private:
	void initMainMenuEntities();
	void initArenas();
	void buildArena1();
	void buildArena2();
	void buildArena3();
	void buildArena4();
	bool winCheck();

	const unsigned int arena1Size = 40;
	const unsigned int arena2Size = 20;
	const unsigned int arena3Size = 30;
	const unsigned int arena4Size = 15;

	glm::vec2 playerStartingPosition;
	glm::vec2 ai1StartingPosition;
	glm::vec2 ai2StartingPosition;
	glm::vec2 ai3StartingPosition;

	render::Camera camera;
	ui::MainMenu mainMenu;
	ui::PauseMenu pauseMenu;
	ui::EndMenu endMenu;
	ui::DevUI devUI;
	ui::LoadingScreen loadingScreen;

	std::vector<std::shared_ptr<entity::Arena>> arenas;
	std::shared_ptr<entity::Arena> currentArena;
	std::unique_ptr<Controller> controller;
	std::shared_ptr<entity::Vehicle> player;

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

	float fps;
	float deltaSec;
	float lastFrame;
	float roundTimer;	// seconds.

	bool reachedTarget1 = false;
	bool reachedTarget2 = false;
	bool reachedTarget3 = false;

	void runMainMenu();
	void endGame(physics::Simulate& simulator, ui::HUD& playerHUD);
	void runGame();
	void loadTextures();
	void initEntities();
	void initDevUI();
	void resetAll();
	void resetTeams();

	void getDevUISettings();
	void getDevUIHandlingSettings(physics::Simulate& simulator);
	void mainMenuAnimation(physics::Simulate& simulator, float animationClock);
	void introScene(float introState, float deltaSec);
};
}	// namespace engine
}	// namespace hyperbright