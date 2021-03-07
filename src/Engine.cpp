#include "Engine.h"

#include <filesystem>
#include <iostream>

#include "Ai.h"
#include "AiManager.h"
#include "AudioPlayer.h"
#include "Controller.h"
#include "DevUI.h"
#include "Pickup.h"
#include "TeamStats.h"

#define STARTGAME 1
#define NOINPUT 0
#define ENDGAME 2
#define LOADOUT 3


Engine::Engine() :
	camera(), renderer(camera),
	deltaSec(0.0f), rotate(0), scale(1),
	lastFrame(0.0f), selection(NOINPUT)
{
	// load textures into a shared pointer.
	loadTextures();
	initEntities();
	setupAudioPlayer();
}


Engine::~Engine() {}

void Engine::setupAudioPlayer() {
	audioPlayer = std::shared_ptr<AudioPlayer>(new AudioPlayer);
}

void Engine::loadTextures()
{
	face = std::make_shared<Texture>("rsc/images/awesomeface.png");
	tree = std::make_shared<Texture>("rsc/images/tree.jpeg");
	background = std::make_shared<Texture>("rsc/images/background.jpg");
}

void Engine::initEntities()
{	
	// Create the player vehicle, setting its starting position, direction, and team (which includes the color of the vehicle/tiles)
	std::shared_ptr<Vehicle> player = std::make_shared<Vehicle>("player", teamStats::Teams::TEAM0, glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
	vehicles.push_back(player);
	renderables.push_back(std::static_pointer_cast<Renderer::IRenderable>(player));
	physicsModels.push_back(std::static_pointer_cast<IPhysical>(player));
	
	// Create the 4 ai vehicles, setting their starting position, direction, and team (which includes the color of the vehicle/tiles)
	
	std::shared_ptr<Vehicle> ai1 = std::make_shared<Vehicle>("ai1", teamStats::Teams::TEAM1, glm::vec3(60.f, 1.f, -60.f), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai1);
	renderables.push_back(std::static_pointer_cast<Renderer::IRenderable>(ai1));
	physicsModels.push_back(std::static_pointer_cast<IPhysical>(ai1));

	std::shared_ptr<Vehicle> ai2 = std::make_shared<Vehicle>("ai2", teamStats::Teams::TEAM2, glm::vec3(40.f, 1.f, -40.f), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai2);
	renderables.push_back(std::static_pointer_cast<Renderer::IRenderable>(ai2));
	physicsModels.push_back(std::static_pointer_cast<IPhysical>(ai2));

	std::shared_ptr<Vehicle> ai3 = std::make_shared<Vehicle>("ai3", teamStats::Teams::TEAM3, glm::vec3(20.f, 1.f, -20.f), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai3);
	renderables.push_back(std::static_pointer_cast<Renderer::IRenderable>(ai3));
	physicsModels.push_back(std::static_pointer_cast<IPhysical>(ai3));

	battery = std::make_shared<Model> ("rsc/models/cube.obj", "battery", nullptr);
	battery->scale(glm::vec3(1.f, 3.f, 1.f));
	battery->translate(glm::vec3(15.f, 1.5f, 5.f));
	battery->update();
	renderables.push_back(battery);
	
	int arena_size = 75;
	//bool aiArenaRepresentation[75][75];
	arena = std::make_shared<Arena>(arena_size, arena_size);
	arena->addWall(0, 0, 2, 2);

	renderables.push_back(arena);
}


/////// Needs to be cleaned up ///////
// Engine::run should initiate the Controller and DevUI 
// then simply keep track of the current window state of
// the game (menu/arena/pause etc) and appropriate func.
void Engine::run()
{
	runMenu();
	runGame();
	endGame();
	return;
}


/*
This Function contains the loop for the main menu.
Responsible for starting and ending the game.
*/
void Engine::runMenu() {

	//***** Initialize anything needed for the main menu HERE *****
	DevUI devUI(renderer.getWindow());
	Controller controller(renderer.getWindow(), camera, vehicles[0], NOINPUT);

	while (!controller.isWindowClosed()) {
		// update global time
		float currentFrame = glfwGetTime();
		deltaSec = currentFrame - lastFrame;

		float fpsLimit = (1.f / devUI.getSliderFPS());

		//wait until a certain amount of time has past
		while (deltaSec < fpsLimit) {
			currentFrame = glfwGetTime();
			deltaSec = currentFrame - lastFrame;
		}
		lastFrame = currentFrame;

		if (controller.isPaused()) {
			deltaSec = 0.f;
		}
		devUI.update(deltaSec);

		// controller 
		controller.processInput(deltaSec);

		// render the updated position of all models and ImGui
		renderer.render(renderables, devUI, selection, controller.isPaused(), controller.getIndex());

		if (controller.stopLoop() == true) {
			selection = STARTGAME;
			break;
		}
		glfwPollEvents();
	}

	//***** Clean up anything needed for the main menu HERE *****

	return;
}
//////////////////////////////////////////////////////////

void Engine::runGame() {
	Simulate simulator(physicsModels, vehicles, *arena);
	simulator.setAudioPlayer(audioPlayer);

	AiManager aiManager;
	//aiManager.setArena(arena->getAiArenaRepresentation());
	aiManager.setArena(arena);
	aiManager.loadAiVehicle(vehicles.at(1));//MUST LOAD EACH VEHICLE CONTROLLED BY AI
	aiManager.loadAiVehicle(vehicles.at(2));
	aiManager.loadAiVehicle(vehicles.at(3));


	DevUI devUI(renderer.getWindow());
	Controller controller(renderer.getWindow(), camera, vehicles[0], STARTGAME);

	// SOUND SETUP
	//audioPlayer->playGameMusic();
	//audioPlayer->playCarIdle();

	while (!controller.isWindowClosed()) {
		// update global time
		float currentFrame = glfwGetTime();
		deltaSec = currentFrame - lastFrame;

		float fpsLimit = (1.f / devUI.getSliderFPS());

		//wait until a certain amount of time has past
		while (deltaSec < fpsLimit) {
			currentFrame = glfwGetTime();
			deltaSec = currentFrame - lastFrame;
		}
		lastFrame = currentFrame;

		if (controller.isPaused()) {
			deltaSec = 0.f;
		}

		devUI.update(deltaSec);

		// controller 
		controller.processInput(deltaSec);

		//AI
		aiManager.makeMoves();

		// run a frame of simulation
		if (!controller.isPaused()) {
			simulator.stepPhysics(fpsLimit);
			simulator.checkVehiclesOverTile(*arena, vehicles);
		}

		// set camera to player vehicles position
		if (!controller.isCameraManual())
		{
			// grab position from player vehicle
			camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getForward());
		}

		// render the updated position of all models and ImGui
		renderer.render(renderables, devUI, selection, controller.isPaused(), controller.getIndex());

		glfwPollEvents();
	}
	audioPlayer->stopGameMusic();
	audioPlayer->stopCarIdle();
	return;
}

//A loop for endgame
void Engine::endGame()
{
	return;
}