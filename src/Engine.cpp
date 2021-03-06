#include "Engine.h"

#include <filesystem>
#include <iostream>

#include "Ai.h"
#include "AudioPlayer.h"
#include "Controller.h"
#include "DevUI.h"
#include "Pickup.h"
#include "AiManager.h"

#define STARTGAME 1
#define NOINPUT 0
#define ENDGAME 2


Engine::Engine() :
	camera(), renderer(camera),
	deltaSec(0.0f), rotate(0), scale(1),
	lastFrame(0.0f)
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
	glm::vec4 playerColor = glm::vec4(.3f, .3f, 1.f, 0.f);
	glm::vec4 ai1Color = glm::vec4(.8f, .8f, .3f, 0.f);
	
	std::shared_ptr<Vehicle> player = std::make_shared<Vehicle>("player", playerColor, glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
	vehicles.push_back(player);
	renderables.push_back(std::static_pointer_cast<Renderer::IRenderable>(player));
	physicsModels.push_back(std::static_pointer_cast<IPhysical>(player));
	

	std::shared_ptr<Vehicle> ai1 = std::make_shared<Vehicle>("ai1", ai1Color, glm::vec3(14.f, 3.f, -14.f), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai1);
	renderables.push_back(std::static_pointer_cast<Renderer::IRenderable>(ai1));
	physicsModels.push_back(std::static_pointer_cast<IPhysical>(ai1));

	/*triggerVolume = loadModel("rsc/models/cube.obj", true, "trigger", nullptr, glm::vec4(.3f, 1.f, .5f, 0.f));
	renderables.push_back(triggerVolume);*/
	
	int arena_size = 75;
	arena = std::make_shared<Arena>(arena_size, arena_size);
	arena->addWall(0, 0, 2, 2);
	arena->addWall(14, 5, 1, 7);
	arena->addWall(4, 17, 5, 2);
	arena->addWall(10, 10, 1, 1);
	renderables.push_back(arena);
}


/////// Needs to be cleaned up ///////
// Engine::run should initiate the Controller and DevUI 
// then simply keep track of the current window state of
// the game (menu/arena/pause etc) and appropriate func.
void Engine::run()
{
	Simulate simulator(physicsModels, vehicles, *arena);
	simulator.setAudioPlayer(audioPlayer);

	AiManager aiManager;
	aiManager.setArena(arena->getAiArenaRepresentation());
	aiManager.setArena(arena);
	aiManager.loadAiVehicle(vehicles.at(1));//MUST LOAD EACH VEHICLE CONTROLLED BY AI

	DevUI devUI(renderer.getWindow());
	Controller controller(renderer.getWindow(), camera, vehicles[0]);

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
		devUI.update(deltaSec);

		// controller 
		controller.processInput(deltaSec);

		//AI
		aiManager.makeMoves();

		// run a frame of simulation
		simulator.stepPhysics(fpsLimit);
		simulator.checkVehiclesOverTile(*arena, vehicles);
		
		/*
		std::cout<< vehicles.at(1)->currentTile.x;
		std::cout << " ";
		std::cout << vehicles.at(1)->currentTile.y;
		std::cout << std::endl;
		*/

		// set camera to player vehicles position
		if (!controller.isCameraManual())
		{
			// grab position from player vehicle
			camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getForward());
		}

		// render the updated position of all models and ImGui
		renderer.render(renderables, devUI);

		glfwPollEvents();
	}
	audioPlayer->stopGameMusic();
	audioPlayer->stopCarIdle();
	//runMenu();
	return;
}


/*
This Function contains the loop for the main menu.
Responsible for starting and ending the game.
*/
void Engine::runMenu() {

	//***** Initialize anything needed for the main menu HERE *****

	int selection = NOINPUT;
	while (1) {
		selection = menuInput();
		// ***** RENDER MENU HERE *****

		if (selection == STARTGAME) {//START GAME
			runGame();
		}
		else if (selection == ENDGAME) {//END GAME
			break;
		}
	}

	//***** Clean up anything needed for the main menu HERE *****

	return;
}

//////////////////////////////////////////////////////////

int Engine::menuInput() {
	//This function will handle input from the user in the menu
	//Returns an int which will indicate the users current selecting.
	//return 0 for no input.
	return 0;
}

//////////////////////////////////////////////////////////

void Engine::runGame() {
	/*
	//***** Initialize game objects HERE *****
	//reset AI to start of game settings
	aiPlayers[1].reset();//ASSUMES 3 AI PLAYERS
	aiPlayers[2].reset();//AI 0 is not used to match vehicle index (for now)
	aiPlayers[3].reset();

	//reset vehicles to start of game settings. (may need coordinates for starting positions as parameter)
	vehicles[0].reset();//Player is 0
	vehicles[1].reset();
	vehicles[2].reset();
	vehicles[3].reset();

	//set up Pickups


	//***** Initialize anything else needed for a game loop*****
	//Simulate simulator;
	//Controller controller;

	//INTIALIZE RENDERER

	while (1) {

		//AI GAME INPUT
		aiPlayers[1].aiInput(vehicles[1]);
		aiPlayers[2].aiInput(vehicles[2]);
		aiPlayers[3].aiInput(vehicles[3]);

		//USER GAME INPUT
		//controller.gameInput(vehicles[0]);

		//SIMULATE FRAME
		//simulator.simulateStep(vehicles, arena, pickups);

		//UI STUFF, SCORE KEEPING

		//RENDER FRAME
		//renderer.renderGameStep(vehicles, arena, pickups);

		if (false) {//QUIT CONDITION NEEDED
			break;
		}
	}
	*/

	//Game loop clean up, before returning to menu
}
