#include "Engine.h"
#include <filesystem>

#define STARTGAME 1
#define NOINPUT 0
#define ENDGAME 2


Engine::Engine() :
	deltaSec(0.0f), rotate(0), scale(1),
	lastFrame(0.0f), modelIndex(0)
{
	camera = std::make_shared<Camera>();
	renderer = std::make_unique<Renderer>(camera);
	loadModels();
	models[modelIndex]->shouldRender = true;
}


Engine::~Engine() {}

/*
 * Loads all models in rsc/models and stores them in a vector. Requires glad
 * to have loaded opengl function calls.
*/
void Engine::loadModels()
{
	namespace fs = std::filesystem;
	const std::string extension = ".obj";

	unsigned int count = 1;
	for (const auto& entry : fs::directory_iterator("rsc/models"))
	{
		if (entry.is_regular_file() && entry.path().extension() == extension)
		{
			std::cout << "Loading " << entry.path() << "..." << std::flush;
			models.push_back(std::make_unique<Model>(entry.path().string()));
			std::cout << "Done! Index: " << count << "\n";
			count++;
		}
	}
}


void Engine::run()
{
	Simulate simulator;
	DevUI devUI(renderer->getWindow());
	Controller controller(renderer->getWindow(), camera);

	while (!controller.isWindowClosed()) {
		// update global time
		float currentFrame = glfwGetTime();
		deltaSec = currentFrame - lastFrame;
		lastFrame = currentFrame;

		controller.processInput(deltaSec);

		//////////////////////////////
		// Temporary model selection
		models[modelIndex]->shouldRender = false;
		modelIndex = controller.modelIndex;
		models[modelIndex]->shouldRender = true;
		models[modelIndex]->update();
		//////////////////////////////

		renderer->render(deltaSec, devUI, models);

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
	Simulate simulator;
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
