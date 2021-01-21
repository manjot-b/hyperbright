#include "Engine.h"
#include "Renderer.h"
#include "Simulate.h"
#include "Arena.h"
#include "Pickup.h"
#include "Ai.h"
#include "Vehicle.h"
#include "DevUI.h"
#include "Controller.h"

#define STARTGAME 1
#define NOINPUT 0
#define ENDGAME 2

//////////////////////////////////////////////////////////

Engine::Engine() {

}

//////////////////////////////////////////////////////////

Engine::~Engine() {

}

//////////////////////////////////////////////////////////

void Engine::run() {

	Renderer renderer;
	renderer.run();

	//runMenu();
	return;
}

//////////////////////////////////////////////////////////

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
	Controller controller;

	//INTIALIZE RENDERER

	while (1) {

		//AI GAME INPUT
		aiPlayers[1].aiInput(vehicles[1]);
		aiPlayers[2].aiInput(vehicles[2]);
		aiPlayers[3].aiInput(vehicles[3]);

		//USER GAME INPUT
		controller.gameInput(vehicles[0]);

		//SIMULATE FRAME 
		simulator.simulateStep(vehicles, arena, pickups);
		
		//UI STUFF, SCORE KEEPING

		//RENDER FRAME
		//renderer.renderGameStep(vehicles, arena, pickups);

		if (false) {//QUIT CONDITION NEEDED
			break;
		}

	}

	//Game loop clean up, before returning to menu
}