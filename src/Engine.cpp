#include "Engine.h"

#include <filesystem>
#include <iostream>

#define STARTGAME 1
#define NOINPUT 0
#define ENDGAME 2


Engine::Engine() :
	deltaSec(0.0f), rotate(0), scale(1),
	lastFrame(0.0f)
{
	camera = std::make_shared<Camera>();
	renderer = std::make_unique<Renderer>(camera);

	// load textures into a shared pointer.
	loadTextures();

	initEntities();
}


Engine::~Engine() {}

/*
 * Loads all models in rsc/models and stores them in a vector. Requires glad
 * to have loaded opengl function calls.
*/
std::shared_ptr<Model> Engine::loadModel(std::string ref, bool inPhysx, Model::MoveType type, const std::shared_ptr<Texture>& texture)
{
	std::shared_ptr<Model> model = std::make_unique<Model>(ref, type, texture);
	std::cout << "Loading " << ref << "..." << std::flush;
	if (inPhysx)
	{
		physicsModels.push_back(model);
	}
	else
	{
		staticModels.push_back(model);
	}
	std::cout << "Loaded Entity " << ref << "\n";
	return model;
}

void Engine::loadTextures()
{
	face = std::make_shared<Texture>("rsc/images/awesomeface.png");
	tree = std::make_shared<Texture>("rsc/images/tree.jpeg");
	background = std::make_shared<Texture>("rsc/images/background.jpg");
}

void Engine::initEntities()
{
	// load boxcar > physicsModels[0]
	vehicle = loadModel("rsc/models/boxcar.obj", true, Model::MoveType::DYNAMIC, face);
	Vehicle player(vehicle, glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(std::make_shared<Vehicle>(player));
	// tmp floor box > staticModels[0]
	grid = loadModel("rsc/models/cube.obj", false, Model::Model::STATIC, tree);
	// background box > staticModels[1]
	skyBox = loadModel("rsc/models/cube.obj", false, Model::Model::STATIC, background);
}


/////// Needs to be cleaned up ///////
// Engine::run should initiate the Controller and DevUI 
// then simply keep track of the current window state of
// the game (menu/arena/pause etc) and appropriate func.
void Engine::run()
{
	Simulate simulator(physicsModels, vehicles);
	DevUI devUI(renderer->getWindow());
	Controller controller(renderer->getWindow(), camera);
  
	// moving the boxcar off origin
	vehicle->translate(glm::vec3(0.0f, 0.0f, -2.0f));

	// temp large box to act as visual floor
	grid->scale(50);
	grid->translate(glm::vec3(0.0f, -25.5f, 0.0f));
	grid->update();

	// tmp huge background box
	skyBox->scale(100);
	skyBox->update();

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

		// controller 
		controller.processInput(deltaSec);
		vehicles[0]->drive(controller.gameInput());

		// run a frame of simulation
		simulator.stepPhysics();
		

		// set camera to player vehicles position
		if (!controller.isCameraManual())
		{
			// grab position from player vehicle
			camera->updateCameraVectors(physicsModels[0]->getPosition());
		}

		// render the updated position of all models and ImGui
		renderer->render(deltaSec, devUI, staticModels, physicsModels);

		glfwPollEvents();
	}

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

	//set up arena
	Arena arena;


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
