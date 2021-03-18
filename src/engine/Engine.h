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

	glm::vec2 playerStartingPosition;
	glm::vec2 ai1StartingPosition;
	glm::vec2 ai2StartingPosition;
	glm::vec2 ai3StartingPosition;

	render::Camera camera;
	ui::Menu menu;
	ui::mainMenu mainmenu;
	ui::pauseMenu pausemenu;
	ui::endMenu endmenu;
	ui::DevUI devUI;
	std::shared_ptr<entity::Arena> arena;
	std::unique_ptr<Controller> controller;

	std::vector<std::shared_ptr<entity::Vehicle>> vehicles;
	std::vector<std::shared_ptr<physics::IPhysical>> physicsModels;
	std::vector<std::shared_ptr<render::Renderer::IRenderable>> renderables;
	std::shared_ptr<openGLHelper::Shader> shader;

	std::shared_ptr<model::Model> battery;
	std::shared_ptr<entity::Pickup> pickup;

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

	void runMainMenu();
	void endGame();
	void runGame();
	void loadTextures();
	void initEntities();
	void setupAudioPlayer();
	void initDevUI();

	void getDevUISettings();
};
}	// namespace engine
}	// namespace hyperbright