#include "Engine.h"
#include <filesystem>
#include <string>

#define STARTGAME 1
#define NOINPUT 0
#define ENDGAME 2


Engine::Engine() :
	deltaSec(0.0f), rotate(0), scale(1),
	lastFrame(0.0f), modelIndex(0)
{
	camera = std::make_shared<Camera>();
	renderer = std::make_unique<Renderer>(camera);

	// load textures into a list
	loadTextures();

	initEntities();
}


Engine::~Engine() {}

/*
 * Loads all models in rsc/models and stores them in a vector. Requires glad
 * to have loaded opengl function calls.
*/
void Engine::loadModels(std::string ref, bool inPhysx, Model::MoveType type)
{
	namespace fs = std::filesystem;
	const std::string extension = ".obj";

	for (const auto& entry : fs::directory_iterator("rsc/models"))
	{
		std::string name = entry.path().stem().string();
		if (name == ref)
		{
			if (entry.is_regular_file() && entry.path().extension() == extension)
			{
				std::cout << "Loading " << entry.path() << "..." << std::flush;
				if (inPhysx)
				{
					physicsModels.push_back(std::make_unique<Model>(entry.path().string(), type));
					physicsModels[physicsModels.size() - 1]->setId(name);  // set the model id to it's file name
				}
				else
				{
					staticModels.push_back(std::make_unique<Model>(entry.path().string(), type));
					staticModels[staticModels.size() - 1]->setId(name);  // set the model id to it's file name
				}
				std::cout << "Loaded Entity " << ref << "\n";
			}
		}
	}
}

void Engine::loadTextures()
{
	// load all textures in images into a list to be selected per entity
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator("rsc/images"))
	{
		textures.push_back(std::make_unique<Texture>(entry.path().string().c_str()));
	}
}

void Engine::initEntities()
{
	// load boxcar > physicsModels[0]
	loadModels("boxcar", true, Model::MoveType::DYNAMIC);
	// tmp floor box > staticModels[0]
	loadModels("cube", false, Model::Model::STATIC);
	// background box > staticModels[1]
	loadModels("cube", false, Model::Model::STATIC);
}


/////// Needs to be cleaned up ///////
// Engine::run should initiate the Controller and DevUI 
// then simply keep track of the current window state of
// the game (menu/arena/pause etc) and appropriate func.
void Engine::run()
{
	Simulate simulator(physicsModels);
	DevUI devUI(renderer->getWindow());
	Controller controller(renderer->getWindow(), camera);
  
	// moving the boxcar off origin
	physicsModels[0]->translate(glm::vec3(0.0f, 0.0f, -2.0f));

	// temp large box to act as visual floor
	staticModels[0]->scale(50);
	staticModels[0]->translate(glm::vec3(0.0f, -25.5f, 0.0f));
	staticModels[0]->update();

	// tmp huge background box
	staticModels[1]->scale(100);
	staticModels[1]->update();

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

		// run a frame of simulation
		simulator.stepPhysics(physicsModels);

		// set camera to player vehicles position
		if (!controller.isCameraManual())
		{
			// grab position from player vehicle
			camera->updateCameraVectors(physicsModels[0]->getPosition());
		}

		// render the updated position of all models and ImGui
		renderer->render(deltaSec, devUI, staticModels, physicsModels, textures);

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

	//Game loop clean up, before returning to menu
}
