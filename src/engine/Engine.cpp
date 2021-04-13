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
	initMainMenuEntities();
	initArenas();
	initDevUI();

	audioPlayer = std::make_shared<audio::AudioPlayer>();
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(),
		camera,
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
	arenas.clear();

	for (unsigned int i = 0; i < teamStats::teamCount; i++)
		teamStats::scores[static_cast<teamStats::Teams>(i)] = 0;

	initMainMenuEntities();
	initArenas();
	initDevUI();
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(),
		camera,
		mainMenu,
		pauseMenu,
		endMenu,
		*audioPlayer);
}

void Engine::initDevUI()
{
	devUI.settings.fps = fps;
	if (vehicles.size() > 0)
		devUI.settings.vehicleBodyMaterial = vehicles[0]->getBodyMaterial();
}

void Engine::loadTextures()
{
	face = std::make_shared<openGLHelper::Texture>("rsc/images/awesomeface.png");
	tree = std::make_shared<openGLHelper::Texture>("rsc/images/tree.jpeg");
	background = std::make_shared<openGLHelper::Texture>("rsc/images/background.jpg");
}

void Engine::initArenas()
{
	using namespace entity;
	arenas = {
		std::make_shared<Arena>(arena1Size, arena1Size, shader, Arena::Difficulty::BEGINNER),
		std::make_shared<Arena>(arena2Size, arena2Size, shader, Arena::Difficulty::NORMAL),
		std::make_shared<Arena>(arena3Size, arena3Size, shader, Arena::Difficulty::NORMAL),
		std::make_shared<Arena>(arena4Size, arena4Size, shader, Arena::Difficulty::HARD)
	};
}

void Engine::buildArena1 () {
	using namespace entity;

	// BUILD ARENA LAYOUT ///////////////////////
	int wallLength = 12;
	int wallWidth = 4;
	arenas[0]->addWall(5, 25, wallLength, wallWidth);
	arenas[0]->addWall(5, 32, wallLength, wallWidth);

	arenas[0]->addWall(5, 5, wallWidth, wallLength);
	arenas[0]->addWall(12, 5, wallWidth, wallLength);

	arenas[0]->addWall(24, 5, wallLength, wallWidth);
	arenas[0]->addWall(24, 12, wallLength, wallWidth);

	arenas[0]->addWall(25, 24, wallWidth, wallLength);
	arenas[0]->addWall(32, 24, wallWidth, wallLength);

	arenas[0]->addWall(0, 0, 1, arena1Size); //left
	arenas[0]->addWall(0, arena1Size - 1, arena1Size, 1);//top

	arenas[0]->addWall(arena1Size - 1, 0, 1, arena1Size - 1);//right
	arenas[0]->addWall(0, 0, arena1Size - 1, 1);//bottom
	////////////////////////////////////////////

	arenas[0]->addChargingStation(20, 20, Arena::Orientation::NEG_Z);
	arenas[0]->addChargingStation(3, 20, Arena::Orientation::POS_Z);
	arenas[0]->addChargingStation(36, 20, Arena::Orientation::NEG_X);
	arenas[0]->addChargingStation(20, 3, Arena::Orientation::POS_X);
	arenas[0]->addChargingStation(20, 36, Arena::Orientation::NEG_Z);

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(18, 18);
	ai1StartingPosition = glm::vec2(18, 24);
	ai2StartingPosition = glm::vec2(24, 18);
	ai3StartingPosition = glm::vec2(24, 24);
	////////////////////////////////////////////
}

void Engine::buildArena2() {
	using namespace entity;

	// BUILD ARENA LAYOUT ///////////////////////

	//CENTER
	//4X1
	arenas[1]->addWall(8, 5, 4, 1);
	arenas[1]->addWall(8, 14, 4, 1);
	//1x1s
	arenas[1]->addWall(6, 7, 1, 1);
	arenas[1]->addWall(7, 6 , 1, 1);

	arenas[1]->addWall(6, 12, 1, 1);
	arenas[1]->addWall(7, 13, 1, 1);

	arenas[1]->addWall(12, 6 , 1, 1);
	arenas[1]->addWall(13, 7, 1, 1);

	arenas[1]->addWall(13, 12, 1, 1);
	arenas[1]->addWall(12, 13, 1, 1);

	//OUTSIDE WALLS
	//2x1
	arenas[1]->addWall(1, 5, 2, 1);
	arenas[1]->addWall(1, 14, 2, 1);
	arenas[1]->addWall(17, 5, 2, 1);
	arenas[1]->addWall(17, 14, 2, 1);
	//1x2
	arenas[1]->addWall(5, 1, 1, 2);
	arenas[1]->addWall(14, 1, 1, 2);
	arenas[1]->addWall(5, 17, 1, 2);
	arenas[1]->addWall(14, 17, 1, 2);

	//WALLS AROUND ARENA
	arenas[1]->addWall(0, 0, 1, arena2Size); //left
	arenas[1]->addWall(0, arena2Size - 1, arena2Size, 1);//top

	arenas[1]->addWall(arena2Size - 1, 0, 1, arena2Size - 1);//right
	arenas[1]->addWall(0, 0, arena2Size - 1, 1);//bottom
	////////////////////////////////////////////

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(8, 8);
	ai1StartingPosition = glm::vec2(11, 8);
	ai2StartingPosition = glm::vec2(8, 11);
	ai3StartingPosition = glm::vec2(11, 11);
	////////////////////////////////////////////
	arenas[1]->addChargingStation(9, 10, Arena::Orientation::NEG_Z);
	arenas[1]->addChargingStation(5, 5, Arena::Orientation::POS_X);
	arenas[1]->addChargingStation(14, 14, Arena::Orientation::POS_X);
}

void Engine::buildArena3() {
	using namespace entity;

	// BUILD ARENA LAYOUT ///////////////////////

	//CENTER
	//2X9
	arenas[2]->addWall(5, 4, 2, 9);
	arenas[2]->addWall(5, 17, 2, 9);
	arenas[2]->addWall(23, 4, 2, 9);
	arenas[2]->addWall(23, 17, 2, 9);

	//10X2
	arenas[2]->addWall(7, 4, 10, 2);
	arenas[2]->addWall(7, 24, 10, 2);
	arenas[2]->addWall(13, 11, 10, 2);
	arenas[2]->addWall(13, 17, 10, 2);
	//2X3
	arenas[2]->addWall(11, 10, 2, 3);
	arenas[2]->addWall(11, 17, 2, 3);
	//2X4
	arenas[2]->addWall(17, 4, 2, 4);
	arenas[2]->addWall(17, 22, 2, 4);

	//WALLS AROUND ARENA
	arenas[2]->addWall(0, 0, 1, arena3Size); //left
	arenas[2]->addWall(0, arena3Size - 1, arena3Size, 1);//top

	arenas[2]->addWall(arena3Size - 1, 0, 1, arena3Size - 1);//right
	arenas[2]->addWall(0, 0, arena3Size - 1, 1);//bottom
	////////////////////////////////////////////

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(15, 9);
	ai1StartingPosition = glm::vec2(15, 8);
	ai2StartingPosition = glm::vec2(16, 8);
	ai3StartingPosition = glm::vec2(14, 8);
	////////////////////////////////////////////
	arenas[2]->addChargingStation(9, 14, Arena::Orientation::NEG_Z);
	arenas[2]->addChargingStation(20, 2, Arena::Orientation::POS_X);
	arenas[2]->addChargingStation(20, 27, Arena::Orientation::POS_X);
}
void Engine::buildArena4() {
	using namespace entity;

	// BUILD ARENA LAYOUT ///////////////////////

	//WALLS AROUND ARENA
	arenas[3]->addWall(0, 0, 1, arena4Size); //left
	arenas[3]->addWall(0, arena4Size - 1, arena4Size, 1);//top

	arenas[3]->addWall(arena4Size - 1, 0, 1, arena4Size - 1);//right
	arenas[3]->addWall(0, 0, arena4Size - 1, 1);//bottom
	////////////////////////////////////////////

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(8, 8);
	ai1StartingPosition = glm::vec2(7, 8);
	ai2StartingPosition = glm::vec2(9, 11);
	ai3StartingPosition = glm::vec2(10, 11);
	////////////////////////////////////////////
	arenas[3]->addChargingStation(7, 7, Arena::Orientation::NEG_Z);
}


void Engine::initMainMenuEntities()
{
	std::shared_ptr<entity::SkyBox> skyBox = std::make_shared<entity::SkyBox>();
	skyBox->rotate(glm::vec3(glm::radians(30.f), 0.f, 0.f));
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(skyBox));

	int arenaSize = 25;
	currentArena = std::make_shared<entity::Arena>(arenaSize, arenaSize, shader, entity::Arena::Difficulty::BEGINNER);
	currentArena->addChargingStation(arenaSize / 2, arenaSize / 2 + 3, entity::Arena::Orientation::POS_Z);
	renderables.push_back(currentArena);
}

void Engine::initEntities()
{	
	std::shared_ptr<entity::SkyBox> skyBox = std::make_shared<entity::SkyBox>();
	skyBox->rotate(glm::vec3(glm::radians(30.f), 0.f, 0.f));
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(skyBox));

	switch (mainMenu.getArenaSelection())
	{
	case ui::MainMenu::ArenaSelection::ARENA1:
		buildArena1();
		currentArena = arenas[0];
		break;
	case ui::MainMenu::ArenaSelection::ARENA2:
		buildArena2();
		currentArena = arenas[1];
		break;
	case ui::MainMenu::ArenaSelection::ARENA3:
		buildArena3();
		currentArena = arenas[2];
		break;
	case ui::MainMenu::ArenaSelection::ARENA4:
		buildArena4();
		currentArena = arenas[3];
		break;
	}

	renderables.push_back(currentArena);

	// Create the player vehicle, setting its starting position, direction, and team (which includes the color of the vehicle/tiles)
	std::shared_ptr<entity::Vehicle> player = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM0, shader, currentArena->getTilePos(playerStartingPosition) + glm::vec3(0, 1.f ,0), glm::vec3(1.f, 0.f, 0.f));
	vehicles.push_back(player);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(player));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(player));
	controller->setPlayerVehicle(player);

	// Create the 4 ai vehicles, setting their starting position, direction, and team (which includes the color of the vehicle/tiles)
	
	std::shared_ptr<entity::Vehicle> ai1 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM1, shader, currentArena->getTilePos(ai1StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai1);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(ai1));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai1));

	std::shared_ptr<entity::Vehicle> ai2 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM2, shader, currentArena->getTilePos(ai2StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai2);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(ai2));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai2));

	std::shared_ptr<entity::Vehicle> ai3 = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM3, shader, currentArena->getTilePos(ai3StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai3);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(ai3));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai3));
}


void Engine::run()
{
	while (!controller->isWindowClosed())
	{
		runMainMenu();
		renderables.clear();	// remove main menu entities
		initEntities();
		initDevUI();

		runGame();

		if (mainMenu.getState() != ui::MainMenu::State::WELCOME) {
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
	while (!controller->isWindowClosed() && mainMenu.getState() != ui::MainMenu::State::OFF) {
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
		
		currentArena->animateChargingStations(currentFrame);

		// render only the menu for now.
		render::Renderer::getInstance().render(renderables, devUI, mainMenu, camera, nullptr);

		getDevUISettings();
		glfwPollEvents();
	}
	audioPlayer->stopStartMenuMusic();
}
//////////////////////////////////////////////////////////

void Engine::runGame() {
	std::shared_ptr<entity::PickupManager> pickupManager = std::make_shared<entity::PickupManager>(audioPlayer, currentArena, &vehicles, renderables);
	pickupManager->initPickups(shader, mainMenu.getArenaSelection());

	physics::Simulate simulator(physicsModels, vehicles, *currentArena, pickupManager);
	simulator.setAudioPlayer(audioPlayer);

	camera.initCameraBoom(glm::vec3(-10.f, 10.f, -25.f), vehicles[0]->getForward());

	ai::AiManager aiManager;
	aiManager.setArena(currentArena, mainMenu.getArenaSelection());//MUST DO BEFORE LOADING VEHICLE
	aiManager.loadAiVehicle(vehicles.at(1));//MUST LOAD EACH VEHICLE CONTROLLED BY AI
	aiManager.loadAiVehicle(vehicles.at(2));
	aiManager.loadAiVehicle(vehicles.at(3));

	ui::HUD playerHUD(vehicles[0], *currentArena, roundTimer);

	audioPlayer->playGameMusic();
	audioPlayer->playCarIdle();

	lastFrame = glfwGetTime();	// Accounts for setup time

	while (!controller->isWindowClosed() && endMenu.getState() != ui::EndMenu::State::ON && mainMenu.getState() != ui::MainMenu::State::WELCOME) {
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

			simulator.stepPhysics(deltaSec);
			simulator.checkVehiclesOverTile(*currentArena, vehicles);

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

			currentArena->animateChargingStations(currentFrame);

			// set camera to player vehicles position
			if (!controller->isCameraManual())
			{
				// grab position from player vehicle
				camera.updateCameraVectors(vehicles[0], deltaSec);
			}
		}

		devUI.update(deltaSec, roundTimer);

		// render the updated position of all models and ImGui
		render::Renderer::getInstance().render(renderables, devUI, pauseMenu, camera, &playerHUD);

		getDevUISettings();
		getDevUIHandlingSettings(simulator);
		glfwPollEvents();
	}
	audioPlayer->stopGameMusic();
	audioPlayer->stopCarIdle();
	simulator.cleanupPhysics();
}
bool Engine::winCheck() {
	int playerScore = teamStats::scores.at(vehicles.at(0)->getTeam());
	if (playerScore > teamStats::scores.at(vehicles.at(1)->getTeam())) {
		if (playerScore > teamStats::scores.at(vehicles.at(2)->getTeam())) {
			if (playerScore > teamStats::scores.at(vehicles.at(3)->getTeam())) {
				return true;
			}
		}
	}
	return false;
}
//A loop for endgame
void Engine::endGame()
{
	if (winCheck()){
		audioPlayer->startWinMusic();
	}
	else {
		audioPlayer->startLossMusic();
	}

	while (!controller->isWindowClosed() && mainMenu.getState() != ui::MainMenu::State::WELCOME) {
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

	if (winCheck()) {
		audioPlayer->stopWinMusic();
	}
	else {
		audioPlayer->stopLossMusic();
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

	camera.setConfigs(
		devUI.settings.cameraHeight,
		devUI.settings.cameraVelocityCoeficient,
		devUI.settings.cameraRestLength,
		devUI.settings.cameraSwingStrength,
		devUI.settings.poiHeight,
		devUI.settings.poiDepth);
}
void Engine::getDevUIHandlingSettings(physics::Simulate simulator)
{
	simulator.setConfigs(devUI.settings.handling);
}
}	// namespace engine
}	// namespace hyperbright