#include "Engine.h"

#include <iostream>

#include "ai/AiManager.h"
#include "entity/SkyBox.h"
#include "TeamStats.h"

namespace hyperbright {
namespace engine {
Engine::Engine() :
	camera(), mainMenu(), pauseMenu(), endMenu(), devUI(render::Renderer::getInstance().getWindow()),
	fps(60.f), deltaSec(0.0f), lastFrame(0.0f), roundTimer(100)
{
	shader = std::make_shared<openGLHelper::Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();
	
	render::Renderer::getInstance().initShaderUniforms(shader);

	// load textures into a shared pointer.
	loadTextures();
	initEntities();
	initDevUI();

	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(), camera, vehicles[0], mainMenu, pauseMenu, endMenu);
	audioPlayer = std::make_shared<audio::AudioPlayer>();
}


Engine::~Engine() {}

void Engine::resetAll()
{
	deltaSec = 0.f;
	lastFrame = 0.f;
	roundTimer = 100.f;
	camera = render::Camera();
	
	vehicles.clear();
	renderables.clear();
	physicsModels.clear();

	for (unsigned int i = 0; i < teamStats::teamCount; i++)
		teamStats::scores[static_cast<teamStats::Teams>(i)] = 0;

	initEntities();
	initDevUI();
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(), camera, vehicles[0], mainMenu, pauseMenu, endMenu);
}

void Engine::initDevUI()
{
	devUI.settings.fps = fps;
	devUI.settings.vehicleBodyMaterial = vehicles[0]->getBodyMaterial();
}

void Engine::loadTextures()
{
	face = std::make_shared<openGLHelper::Texture>("rsc/images/awesomeface.png");
	tree = std::make_shared<openGLHelper::Texture>("rsc/images/tree.jpeg");
	background = std::make_shared<openGLHelper::Texture>("rsc/images/background.jpg");
}

void Engine::buildArena1 () {
	int arena_size = 40;
	arena = std::make_shared<entity::Arena>(arena_size, arena_size, shader);

	// BUILD ARENA LAYOUT ///////////////////////
	int wallLength = 12;
	int wallWidth = 4;
	arena->addWall(25, 5, wallLength, wallWidth);
	arena->addWall(32, 5, wallLength, wallWidth);

	arena->addWall(5, 5, wallWidth, wallLength);
	arena->addWall(5, 12, wallWidth, wallLength);

	arena->addWall(5, 24, wallLength, wallWidth);
	arena->addWall(12, 24, wallLength, wallWidth);

	arena->addWall(24, 25, wallWidth, wallLength);
	arena->addWall(24, 32, wallWidth, wallLength);

	arena->addWall(0, 0, 1, arena_size); //top
	arena->addWall(0, arena_size - 1, 1, arena_size);//bottom

	arena->addWall(0, 1, arena_size - 1, 1);//left
	arena->addWall(arena_size - 1, 0, arena_size - 1, 1);//right
	////////////////////////////////////////////

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(18, 18);
	ai1StartingPosition = glm::vec2(18, 24);
	ai2StartingPosition = glm::vec2(24, 18);
	ai3StartingPosition = glm::vec2(24, 24);
	////////////////////////////////////////////
}

void Engine::initEntities()
{	
	std::shared_ptr<entity::SkyBox> skyBox = std::make_shared<entity::SkyBox>();
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(skyBox));

	buildArena1();//WE CAN HAVE DIFFERENT FUNCTIONS FOR DIFFERENT ARENA BUILDS
	renderables.push_back(arena);

	// Create the player vehicle, setting its starting position, direction, and team (which includes the color of the vehicle/tiles)
	std::shared_ptr<entity::Vehicle> player = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM0, shader, arena->getTilePos(playerStartingPosition) + glm::vec3(0, 1.f ,0), glm::vec3(1.f, 0.f, 0.f));
	vehicles.push_back(player);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(player));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(player));
	
	// Create the 4 ai vehicles, setting their starting position, direction, and team (which includes the color of the vehicle/tiles)
	
	std::shared_ptr<entity::Vehicle> ai1 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM1, shader, arena->getTilePos(ai1StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai1);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(ai1));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai1));

	std::shared_ptr<entity::Vehicle> ai2 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM2, shader, arena->getTilePos(ai2StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai2);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(ai2));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai2));

	std::shared_ptr<entity::Vehicle> ai3 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM3, shader, arena->getTilePos(ai3StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai3);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(ai3));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai3));

	battery = std::make_shared<model::Model> ("rsc/models/cube.obj", "battery", shader, background);
	battery->scale(glm::vec3(1.f, 3.f, 1.f));
	battery->translate(glm::vec3(15.f, 1.5f, 5.f));
	battery->update();
	battery->getMeshes()[0]->material.color = glm::vec4(1.f, 0.f, 0.f, 1.f);
	renderables.push_back(battery);
}


void Engine::run()
{
	while (!controller->isWindowClosed())
	{
		runMainMenu();
		runGame();

		if (mainMenu.getState() != ui::MainMenu::State::ON) {
			endGame();

			if (!controller->isWindowClosed()) {	// user selected to go to main menu from end menu.
				resetAll();
			}
		}
		else {	// user selected to go to main menu from pause menu.
			resetAll();
		}
	}
}


/*
This Function contains the loop for the main menu.
*/
void Engine::runMainMenu() {
	audioPlayer->playStartMenuMusic();
	while (!controller->isWindowClosed() && mainMenu.getState() == ui::MainMenu::State::ON) {
		// update global time
		float currentFrame = glfwGetTime();
		deltaSec = currentFrame - lastFrame;

		float fpsLimit = (1.f / fps);

		//wait until a certain amount of time has past
		while (deltaSec < fpsLimit) {
			currentFrame = glfwGetTime();
			deltaSec = currentFrame - lastFrame;
		}
		lastFrame = currentFrame;

		devUI.update(deltaSec, roundTimer);
		controller->processInput(deltaSec);	// will update the menu state once ENTER is pressed.

		// render only the menu for now.
		render::Renderer::getInstance().render(renderables, devUI, mainMenu, camera);

		getDevUISettings();
		glfwPollEvents();
	}
	audioPlayer->stopStartMenuMusic();
}
//////////////////////////////////////////////////////////

void Engine::runGame() {
	std::shared_ptr<entity::PickupManager> pickupManager = std::make_shared<entity::PickupManager>(arena, &vehicles, renderables);
	pickupManager->initPickups(shader);

	physics::Simulate simulator(physicsModels, vehicles, *arena, pickupManager);
	simulator.setAudioPlayer(audioPlayer);

	ai::AiManager aiManager;
	aiManager.setArena(arena);
	aiManager.loadAiVehicle(vehicles.at(1));//MUST LOAD EACH VEHICLE CONTROLLED BY AI
	aiManager.loadAiVehicle(vehicles.at(2));
	aiManager.loadAiVehicle(vehicles.at(3));

	//audioPlayer->playGameMusic();
	//audioPlayer->playCarIdle();

	while (!controller->isWindowClosed() && endMenu.getState() != ui::EndMenu::State::ON && mainMenu.getState() != ui::MainMenu::State::ON) {
		// update global time
		float currentFrame = glfwGetTime();
		deltaSec = currentFrame - lastFrame;

		float fpsLimit = (1.f / fps);

		//wait until a certain amount of time has past
		while (deltaSec < fpsLimit) {
			currentFrame = glfwGetTime();
			deltaSec = currentFrame - lastFrame;
		}
		lastFrame = currentFrame;

		controller->processInput(deltaSec);

		//AI
		aiManager.makeMoves();

		if (pauseMenu.getState() != ui::PauseMenu::State::ON) {
			roundTimer -= deltaSec;
			if (roundTimer < 0.01f)
				endMenu.setState(ui::EndMenu::State::ON);

			simulator.stepPhysics(fpsLimit);
			simulator.checkVehiclesOverTile(*arena, vehicles);

			// check for pickups to be added to scene
			while (pickupManager->toBeAddedPickups.size() > 0) {
				physics::addPickup(pickupManager->toBeAddedPickups.front());	// adds physx actor
				renderables.push_back(pickupManager->toBeAddedPickups.front());	// adds model to renderables
				pickupManager->toBeAddedPickups.pop();
			}

			// check for pickups that have been removed from renderables and remove their physx actors
			physics::removePickups();

			// check state of all pickups
			pickupManager->checkPickups();
			pickupManager->animatePickups(fpsLimit);

			// set camera to player vehicles position
			if (!controller->isCameraManual())
			{
				// grab position from player vehicle
				camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getForward());
			}
		}

		devUI.update(deltaSec, roundTimer);

		// render the updated position of all models and ImGui
		render::Renderer::getInstance().render(renderables, devUI, pauseMenu, camera);

		getDevUISettings();
		glfwPollEvents();
	}
	audioPlayer->stopGameMusic();
	audioPlayer->stopCarIdle();
	simulator.cleanupPhysics();
}

//A loop for endgame
void Engine::endGame()
{
	while (!controller->isWindowClosed() && mainMenu.getState() != ui::MainMenu::State::ON) {
		// update global time
		float currentFrame = glfwGetTime();
		deltaSec = currentFrame - lastFrame;

		float fpsLimit = (1.f / fps);

		//wait until a certain amount of time has past
		while (deltaSec < fpsLimit) {
			currentFrame = glfwGetTime();
			deltaSec = currentFrame - lastFrame;
		}
		lastFrame = currentFrame;

		devUI.update(deltaSec, roundTimer);
		controller->processInput(deltaSec);	// will update the menu state once ENTER is pressed.

		// render only the menu for now.
		render::Renderer::getInstance().render(renderables, devUI, endMenu, camera);
		
		getDevUISettings();
		glfwPollEvents();
	}
}

void Engine::getDevUISettings() {
	fps = devUI.settings.fps;

	for (unsigned int i = 0; i < vehicles.size(); i++)
	{
		// We don't want all the vehicles to have the same color.
		devUI.settings.vehicleBodyMaterial.color = teamStats::colors.at(static_cast<teamStats::Teams>(i));
		vehicles[i]->setBodyMaterial(devUI.settings.vehicleBodyMaterial);
	}
}
}	// namespace engine
}	// namespace hyperbright