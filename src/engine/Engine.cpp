#include "Engine.h"

#include <iostream>

#include "ai/AiManager.h"
#include "entity/SkyBox.h"
#include "TeamStats.h"

namespace hyperbright {
namespace engine {
Engine::Engine() :
	camera(), mainMenu(), pauseMenu(), endMenu(), playerHUD(0, 0), devUI(render::Renderer::getInstance().getWindow()),
	fps(60.f), deltaSec(0.0f), lastFrame(0.0f), roundTimer(100)
{
	shader = std::make_shared<openGLHelper::Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();
	
	render::Renderer::getInstance().initShaderUniforms(shader);

	// load textures into a shared pointer.
	loadTextures();
	initEntities();
	initDevUI();

	audioPlayer = std::make_shared<audio::AudioPlayer>();
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(),
		camera,
		vehicles[0],
		mainMenu,
		pauseMenu,
		endMenu,
		*audioPlayer);
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
	playerHUD.update(0, 0);
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(),
		camera,
		vehicles[0],
		mainMenu,
		pauseMenu,
		endMenu,
		*audioPlayer);
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
	using namespace entity;

	int arena_size = 40;
	arena = std::make_shared<Arena>(arena_size, arena_size, shader);

	// BUILD ARENA LAYOUT ///////////////////////
	int wallLength = 12;
	int wallWidth = 4;
	arena->addWall(5, 25, wallLength, wallWidth);
	arena->addWall(5, 32, wallLength, wallWidth);

	arena->addWall(5, 5, wallWidth, wallLength);
	arena->addWall(12, 5, wallWidth, wallLength);

	arena->addWall(24, 5, wallLength, wallWidth);
	arena->addWall(24, 12, wallLength, wallWidth);

	arena->addWall(25, 24, wallWidth, wallLength);
	arena->addWall(32, 24, wallWidth, wallLength);

	arena->addWall(0, 0, 1, arena_size); //left
	arena->addWall(0, arena_size - 1, arena_size, 1);//top

	arena->addWall(arena_size - 1, 0, 1, arena_size - 1);//right
	arena->addWall(0, 0, arena_size - 1, 1);//bottom
	////////////////////////////////////////////

	arena->addChargingStation(20, 20, Arena::Orientation::NEG_Z);
	arena->addChargingStation(3, 20, Arena::Orientation::POS_Z);
	arena->addChargingStation(36, 20, Arena::Orientation::NEG_X);
	arena->addChargingStation(20, 3, Arena::Orientation::POS_X);
	arena->addChargingStation(20, 36, Arena::Orientation::NEG_Z);

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(18, 18);
	ai1StartingPosition = glm::vec2(18, 24);
	ai2StartingPosition = glm::vec2(24, 18);
	ai3StartingPosition = glm::vec2(24, 24);
	////////////////////////////////////////////
}

void Engine::buildArena2() {
	using namespace entity;

	int arena_size = 20;
	arena = std::make_shared<entity::Arena>(arena_size, arena_size, shader);

	// BUILD ARENA LAYOUT ///////////////////////

	//CENTER
	//4X1
	arena->addWall(8, 5, 4, 1);
	arena->addWall(8, 14, 4, 1);
	//1x1s
	arena->addWall(6, 7, 1, 1);
	arena->addWall(7, 6 , 1, 1);

	arena->addWall(6, 12, 1, 1);
	arena->addWall(7, 13, 1, 1);

	arena->addWall(12, 6 , 1, 1);
	arena->addWall(13, 7, 1, 1);

	arena->addWall(13, 12, 1, 1);
	arena->addWall(12, 13, 1, 1);

	//OUTSIDE WALLS
	//2x1
	arena->addWall(1, 5, 2, 1);
	arena->addWall(1, 14, 2, 1);
	arena->addWall(17, 5, 2, 1);
	arena->addWall(17, 14, 2, 1);
	//1x2
	arena->addWall(5, 1, 1, 2);
	arena->addWall(14, 1, 1, 2);
	arena->addWall(5, 17, 1, 2);
	arena->addWall(14, 17, 1, 2);

	//WALLS AROUND ARENA
	arena->addWall(0, 0, 1, arena_size); //left
	arena->addWall(0, arena_size - 1, arena_size, 1);//top

	arena->addWall(arena_size - 1, 0, 1, arena_size - 1);//right
	arena->addWall(0, 0, arena_size - 1, 1);//bottom
	////////////////////////////////////////////

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(8, 8);
	ai1StartingPosition = glm::vec2(11, 8);
	ai2StartingPosition = glm::vec2(8, 11);
	ai3StartingPosition = glm::vec2(11, 11);
	////////////////////////////////////////////
	arena->addChargingStation(9, 10, Arena::Orientation::NEG_Z);
	arena->addChargingStation(5, 5, Arena::Orientation::POS_X);
	arena->addChargingStation(14, 14, Arena::Orientation::POS_X);
}




void Engine::initEntities()
{	
	std::shared_ptr<entity::SkyBox> skyBox = std::make_shared<entity::SkyBox>();
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(skyBox));

	currentArena = 2;
	if (currentArena == 1) {
		buildArena1();
	}
	else if (currentArena == 2) {
		buildArena2();
	}

	renderables.push_back(arena);

	// Create the player vehicle, setting its starting position, direction, and team (which includes the color of the vehicle/tiles)
	std::shared_ptr<entity::Vehicle> player = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM0, shader, arena->getTilePos(playerStartingPosition) + glm::vec3(0, 1.f ,0), glm::vec3(1.f, 0.f, 0.f));
	vehicles.push_back(player);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(player));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(player));
	
	// Create the 4 ai vehicles, setting their starting position, direction, and team (which includes the color of the vehicle/tiles)
	
	std::shared_ptr<entity::Vehicle> ai1 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM1, shader, arena->getTilePos(ai1StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai1);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(ai1));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai1));

	std::shared_ptr<entity::Vehicle> ai2 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM2, shader, arena->getTilePos(ai2StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai2);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(ai2));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai2));

	std::shared_ptr<entity::Vehicle> ai3 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM3, shader, arena->getTilePos(ai3StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai3);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(ai3));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai3));	
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
* 
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
		render::Renderer::getInstance().render(renderables, devUI, mainMenu, camera, nullptr);

		getDevUISettings();
		glfwPollEvents();
	}
	audioPlayer->stopStartMenuMusic();
}
//////////////////////////////////////////////////////////

void Engine::runGame() {
	std::shared_ptr<entity::PickupManager> pickupManager = std::make_shared<entity::PickupManager>(arena, &vehicles, renderables);
	pickupManager->initPickups(shader, currentArena);

	physics::Simulate simulator(physicsModels, vehicles, *arena, pickupManager);
	simulator.setAudioPlayer(audioPlayer);

	ai::AiManager aiManager;
	aiManager.setArena(arena, currentArena);//MUST DO BEFORE LOADING VEHICLE
	aiManager.loadAiVehicle(vehicles.at(1));//MUST LOAD EACH VEHICLE CONTROLLED BY AI
	aiManager.loadAiVehicle(vehicles.at(2));
	aiManager.loadAiVehicle(vehicles.at(3));

	audioPlayer->playGameMusic();
	audioPlayer->playCarIdle();

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

			audioPlayer->adjustCarIdlePitch(vehicles.at(0)->readSpeedometer());

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

			arena->animateChargingStations(currentFrame);

			// set camera to player vehicles position
			if (!controller->isCameraManual())
			{
				// grab position from player vehicle
				camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getForward());
			}
		}

		devUI.update(deltaSec, roundTimer);

		//HUD
		playerHUD.update(vehicles[0]->readSpeedometer(), vehicles[0]->energy);
		playerHUD.updateTime(roundTimer);

		// render the updated position of all models and ImGui
		render::Renderer::getInstance().render(renderables, devUI, pauseMenu, camera, &playerHUD);

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
		render::Renderer::getInstance().render(renderables, devUI, endMenu, camera, nullptr);
		
		getDevUISettings();
		glfwPollEvents();
	}
}

void Engine::getDevUISettings() {
	fps = devUI.settings.fps;
	audioPlayer->setMusicVolume(devUI.settings.musicVolume);
	for (unsigned int i = 0; i < vehicles.size(); i++)
	{
		// We don't want all the vehicles to have the same color.
		devUI.settings.vehicleBodyMaterial.color = teamStats::colors.at(static_cast<teamStats::Teams>(i));
		vehicles[i]->setBodyMaterial(devUI.settings.vehicleBodyMaterial);
	}
}
}	// namespace engine
}	// namespace hyperbright