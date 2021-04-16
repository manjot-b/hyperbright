#include "Engine.h"

#include <iostream>

#include "ai/AiManager.h"
#include "entity/SkyBox.h"
#include "TeamStats.h"

namespace hyperbright {
namespace engine {
Engine::Engine() :
	// need to make sure Renderer is instantiated first to setup up OpenGL context
	devUI(render::Renderer::getInstance().getWindow()), camera(), mainMenu(arenas), pauseMenu(), endMenu(),
	loadingScreen(), fps(60.f), deltaSec(0.0f), lastFrame(0.0f), roundTimer(100)
{
	shader = std::make_shared<openGLHelper::Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();
	
	render::Renderer::getInstance().initShaderUniforms(shader);

	// load textures into a shared pointer.
	loadTextures();


	audioPlayer = std::make_shared<audio::AudioPlayer>();
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(),
		camera,
		mainMenu,
		pauseMenu,
		endMenu,
		loadingScreen,
		*audioPlayer);

	initMainMenuEntities();
	initArenas();
	initDevUI();
}


Engine::~Engine() {}

void Engine::resetAll()
{
	deltaSec = 0.f;
	lastFrame = 0.f;
	roundTimer = 100.f;
	camera = render::Camera();

	reachedTarget1 = false;
	reachedTarget2 = false;
	reachedTarget3 = false;
	
	vehicles.clear();
	renderables.clear();
	physicsModels.clear();
	arenas.clear();

	for (unsigned int i = 0; i < teamStats::teamCount; i++)
		teamStats::scores[static_cast<teamStats::Teams>(i)] = 0;

	loadingScreen.setState(ui::LoadingScreen::State::LOADING1);

	initMainMenuEntities();
	initArenas();
	initDevUI();
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(),
		camera,
		mainMenu,
		pauseMenu,
		endMenu,
		loadingScreen,
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

	camera.setCameraPostion(glm::vec3(-10.f, 3.0f, 0.f));
	camera.setYawAndPitch(-75.f, 0.f);

	camera.updateCameraVectors();

	std::shared_ptr<entity::SkyBox> skyBox = std::make_shared<entity::SkyBox>();
	skyBox->rotate(glm::vec3(glm::radians(30.f), 0.f, 0.f));
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(skyBox));
	render::Renderer::getInstance().setSkyboxCubeMap(skyBox->getCubeMap());

	int arenaSize = 25;
	currentArena = std::make_shared<entity::Arena>(arenaSize, arenaSize, shader, entity::Arena::Difficulty::BEGINNER);

	currentArena->addChargingStation(arenaSize / 2 - 1, arenaSize / 2 + 5, entity::Arena::Orientation::POS_Z);
	currentArena->addWall(arenaSize / 2 + 3, arenaSize / 2 + 1, 2, 7);

	renderables.push_back(currentArena);

	// Create the player vehicle, setting its starting position, direction, and team (which includes the color of the vehicle/tiles)
	player = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM0, shader, currentArena->getTilePos(glm::vec2(15, 9)) + glm::vec3(0, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
	vehicles.push_back(player);
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(player));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(player));
	controller->setPlayerVehicle(player);
}

void Engine::initEntities()
{	
	std::shared_ptr<entity::SkyBox> skyBox = std::make_shared<entity::SkyBox>();
	skyBox->rotate(glm::vec3(glm::radians(30.f), 0.f, 0.f));
	renderables.push_back(std::static_pointer_cast<render::IRenderable>(skyBox));
	render::Renderer::getInstance().setSkyboxCubeMap(skyBox->getCubeMap());

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
	player = std::make_shared<entity::Vehicle>(teamStats::Teams::TEAM0, shader, currentArena->getTilePos(playerStartingPosition) + glm::vec3(0, 1.f ,0), glm::vec3(1.f, 0.f, 0.f));
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
		if (mainMenu.getSelection() == ui::MainMenu::Selection::EXIT)
			break;

		render::Renderer::getInstance().render(loadingScreen);
		renderables.clear();	// remove main menu entities
		vehicles.clear();
		physicsModels.clear();

		initEntities();
		initDevUI();

		runGame();

		if (mainMenu.getState() != ui::MainMenu::State::WELCOME) {

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
	std::shared_ptr<entity::PickupManager> pickupManager = std::make_shared<entity::PickupManager>(audioPlayer, currentArena, &vehicles, renderables);
	physics::Simulate simulator(physicsModels, vehicles, *currentArena, pickupManager);

	audioPlayer->playStartMenuMusic();
	float startTimer = glfwGetTime();
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

		float animationClock = glfwGetTime() - startTimer;
		mainMenuAnimation(simulator, animationClock);

		simulator.stepPhysics(fpsLimit);
		simulator.checkVehiclesOverTile(*currentArena, vehicles);

		devUI.update(deltaSec, roundTimer);
		controller->processInput(fpsLimit);	// will update the menu state once ENTER is pressed.
		
		currentArena->animateChargingStations(currentFrame);

		// render only the menu for now.
		render::Renderer::getInstance().render(renderables, devUI, mainMenu, camera, nullptr);

		getDevUISettings();
		glfwPollEvents();
	}
	audioPlayer->stopStartMenuMusic();
	simulator.cleanupPhysics();
}
//////////////////////////////////////////////////////////

void Engine::runGame() {
	for (unsigned int i = 0; i < teamStats::teamCount; i++)
		teamStats::scores[static_cast<teamStats::Teams>(i)] = 0;

	std::shared_ptr<entity::PickupManager> pickupManager = std::make_shared<entity::PickupManager>(audioPlayer, currentArena, &vehicles, renderables);
	pickupManager->initPickups(shader, mainMenu.getArenaSelection());

	physics::Simulate simulator(physicsModels, vehicles, *currentArena, pickupManager);
	simulator.setAudioPlayer(audioPlayer);

	// give enough time to read game rules.
	float loadingMinWait = 5.f;
	while (glfwGetTime() - lastFrame < loadingMinWait);

	// Loading 2/3 complete
	loadingScreen.setState(ui::LoadingScreen::State::LOADING2);
	render::Renderer::getInstance().render(loadingScreen);

	camera.initCameraBoom(glm::vec3(10.f, 5.f, 10.f), vehicles[0]->getDirection());

	ai::AiManager aiManager;
	aiManager.setArena(currentArena, mainMenu.getArenaSelection());//MUST DO BEFORE LOADING VEHICLE
	aiManager.loadAiVehicle(vehicles.at(1));//MUST LOAD EACH VEHICLE CONTROLLED BY AI
	aiManager.loadAiVehicle(vehicles.at(2));
	aiManager.loadAiVehicle(vehicles.at(3));

	ui::HUD playerHUD(vehicles[0], *currentArena, roundTimer);

	// Loading complete. Wait for user input.
	loadingScreen.setState(ui::LoadingScreen::State::WAITING);
	render::Renderer::getInstance().render(loadingScreen);
	while (loadingScreen.getState() != ui::LoadingScreen::State::DONE)
	{
		controller->processInput(0);	// needed for gamepad button presses.
		glfwPollEvents();
	}

	audioPlayer->playGameMusic();
	audioPlayer->playCarIdle();

	lastFrame = glfwGetTime();	// Accounts for setup time
	float introState = 5.f;

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

		if (introState > 0.f) {
			if (pauseMenu.getState() != ui::PauseMenu::State::ON) {
				if (introState < 1.f) playerHUD.setState(ui::HUD::State::PLAY);
				playerHUD.startCountdown = true;
				playerHUD.countdownTimer = introState;

				simulator.stepPhysics(fpsLimit);
				simulator.checkVehiclesOverTile(*currentArena, vehicles);

				introScene(introState, fpsLimit);
				introState -= deltaSec;
			}
		}
		else {
		
			controller->processInput(fpsLimit);

			//AI
			aiManager.makeMoves();

			if (pauseMenu.getState() != ui::PauseMenu::State::ON) {
				roundTimer -= deltaSec;
				if (roundTimer < 0.01f)
					endMenu.setState(ui::EndMenu::State::ON);

				audioPlayer->adjustCarIdlePitch(vehicles.at(0)->readSpeedometer());

				simulator.stepPhysics(fpsLimit);
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
					camera.updateCameraVectors(vehicles[0], fpsLimit);
				}
			}
		}

		devUI.update(deltaSec, roundTimer);

		// render the updated position of all models and ImGui
		render::Renderer::getInstance().render(renderables, devUI, pauseMenu, camera, &playerHUD);

		getDevUISettings();
		getDevUIHandlingSettings(simulator);
		glfwPollEvents();
	}
	if (endMenu.getState() == ui::EndMenu::State::ON) {
		endGame(simulator, playerHUD);
	}
	resetTeams();
	audioPlayer->stopGameMusic();
	audioPlayer->stopCarIdle();
	simulator.cleanupPhysics();
}

void Engine::resetTeams(){
	vehicles.at(0)->setTeam(engine::teamStats::Teams::TEAM0);
	vehicles.at(1)->setTeam(engine::teamStats::Teams::TEAM1);
	vehicles.at(2)->setTeam(engine::teamStats::Teams::TEAM2);
	vehicles.at(3)->setTeam(engine::teamStats::Teams::TEAM3);
}


bool Engine::winCheck() {
	return std::get<0>(teamStats::sortedScores().front()) == teamStats::Teams::TEAM0;
}
//A loop for endgame
void Engine::endGame(physics::Simulate& simulator, ui::HUD& playerHUD)
{
	audioPlayer->stopGameMusic();
	audioPlayer->stopCarIdle();
	playerHUD.setState(ui::HUD::State::OUTRO);

	if (winCheck()){
		audioPlayer->startWinMusic();
		playerHUD.victory = true;
	}
	else {
		audioPlayer->startLossMusic();
		playerHUD.victory = false;
	}

	for (auto vehicle : vehicles) {
		vehicle->resetControls();
	}
	
	glm::vec3 circle;
	float theta = 0.f;
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

		theta += deltaSec;
		camera.setBoomRestLength(3.f * abs(sin(theta/4.f)) + 8.f);
		camera.setCamHeight(3.5f);
		circle = { vehicles[0]->getPosition().x - cos(theta/2.f), 0.f, vehicles[0]->getPosition().z - sin(theta/2.f) };
		camera.updateCameraVectors(vehicles[0]->getPosition(), circle, deltaSec);

		devUI.update(deltaSec, roundTimer);
		controller->processInput(deltaSec);	// will update the menu state once ENTER is pressed.

		for (int i = 0; i < vehicles.size(); i++) {

			simulator.applyStoppingForce(i);
		}
		simulator.stepPhysics(fpsLimit);

		// render only the menu for now.
		render::Renderer::getInstance().render(renderables, devUI, endMenu, camera, &playerHUD);
		
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

void Engine::getDevUIHandlingSettings(physics::Simulate& simulator)
{
	simulator.setConfigs(devUI.settings.handling);
}

void Engine::mainMenuAnimation(physics::Simulate& simulator, float animationClock)
{
	if (reachedTarget3) {
		simulator.applyStoppingForce(0);
		return;
	}

	glm::vec3 target1 = currentArena->getTilePos(glm::vec2(15, 12));
	glm::vec3 target2 = currentArena->getTilePos(glm::vec2(20, 12));
	glm::vec3 target3 = currentArena->getTilePos(glm::vec2(14, 11));

	if (animationClock > 1.5f) {
		if (!reachedTarget1) {
			reachedTarget3 = false;
			glm::vec3 targetDir = glm::normalize(target1 - player->getPosition());
			float projP_T = glm::dot(targetDir, player->getDirection());
			if (projP_T < 0.99f) {
				glm::vec3 perpP_T = glm::cross(targetDir, player->getDirection());
				if (perpP_T.y > 0.f) { // turn right
					player->stopLeft();
					player->turnRight();
					simulator.applyIntroForce(15);
				}
				else if (perpP_T.y <= 0.f) { // turn left
					player->stopRight();
					player->turnLeft();
					simulator.applyIntroForce(15);
				}
			}
			else {
				player->stopRight();
				player->stopLeft();
				simulator.applyIntroForce(35);
			}

			if (glm::length(target1 - player->getPosition()) < 3.f) reachedTarget1 = true;
		}
		else if (!reachedTarget2) {
			glm::vec3 targetDir = glm::normalize(target2 - player->getPosition());
			float projP_T = glm::dot(targetDir, player->getDirection());
			if (projP_T < 0.99f) {
				glm::vec3 perpP_T = glm::cross(targetDir, player->getDirection());
				if (perpP_T.y > 0.f) { // turn right
					player->stopLeft();
					player->turnRight();
					simulator.applyIntroForce(15);
				}
				else if (perpP_T.y < 0.f) { // turn left
					player->stopRight();
					player->turnLeft();
					simulator.applyIntroForce(15);
				}
			}
			else {
				player->stopRight();
				player->stopLeft();
				simulator.applyIntroForce(30);
			}

			if (glm::length(target2 - player->getPosition()) < 3.f) reachedTarget2 = true;
		}
		else if (!reachedTarget3) {
			glm::vec3 targetDir = glm::normalize(target3 - player->getPosition());
			float projP_T = glm::dot(targetDir, player->getDirection());
			if (projP_T < 0.99f) {
				glm::vec3 perpP_T = glm::cross(targetDir, player->getDirection());
				if (perpP_T.y > 0.f) { // turn right
					player->stopLeft();
					player->turnRight();
					simulator.applyIntroForce(15);
				}
				else if (perpP_T.y < 0.f) { // turn left
					player->stopRight();
					player->turnLeft();
					simulator.applyIntroForce(15);
				}
			}
			else {
				player->stopRight();
				player->stopLeft();
				simulator.applyIntroForce(30);
			}

			if (glm::length(target3 - player->getPosition()) < 5.f) reachedTarget3 = true;
		}
	}
}

void Engine::introScene(float introState, float deltaSec)
{
	if (introState < 1.f) {
		camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getPosition() + vehicles[0]->getDirection() * 0.01f, deltaSec);
	}
	else if (introState < 2.f) {
		camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getPosition() - vehicles[0]->getDirection() * 0.01f, deltaSec);
	}
	else if (introState < 3.f) {
		camera.updateCameraVectors(vehicles[3]->getPosition(), vehicles[3]->getPosition() - vehicles[3]->getDirection() * 0.01f, deltaSec);
	}
	else if (introState < 4.f) {
		camera.updateCameraVectors(vehicles[2]->getPosition(), vehicles[2]->getPosition() - vehicles[2]->getDirection() * 0.01f, deltaSec);
	}
	else {
		camera.updateCameraVectors(vehicles[1]->getPosition(), vehicles[1]->getPosition() - vehicles[1]->getDirection() * 0.01f, deltaSec);
	}
}
}	// namespace engine
}	// namespace hyperbright