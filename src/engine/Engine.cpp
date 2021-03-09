#include "Engine.h"

#include <iostream>

#include "ai/AiManager.h"
#include "TeamStats.h"

Engine::Engine() :
	camera(), menu(), devUI(Renderer::getInstance().getWindow()),
	deltaSec(0.0f), lastFrame(0.0f), roundTimer(60)
{
	// load textures into a shared pointer.
	loadTextures();
	initEntities();
	setupAudioPlayer();

	controller = std::make_unique<Controller>(Renderer::getInstance().getWindow(), camera, vehicles[0], menu);
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
	arena->addWall(35, 35, 1, 7);
	arena->addWall(45, 45, 5, 2);

	renderables.push_back(arena);
}


void Engine::run()
{
	runMainMenu();
	runGame();
	endGame();
}


/*
This Function contains the loop for the main menu.
*/
void Engine::runMainMenu() {
	audioPlayer->playStartMenuMusic();
	while (menu.getState() == Menu::State::MAIN) {
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

		devUI.update(deltaSec, roundTimer);
		controller->processInput(deltaSec);	// will update the menu state once ENTER is pressed.

		// render only the menu for now.
		Renderer::getInstance().render(renderables, devUI, menu, camera);

		glfwPollEvents();
	}
	audioPlayer->stopStartMenuMusic();
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

	audioPlayer->playGameMusic();
	audioPlayer->playCarIdle();

	while (!controller->isWindowClosed() && menu.getState() != Menu::State::END) {
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


		controller->processInput(deltaSec);

		//AI
		aiManager.makeMoves();

		// run a frame of simulation
		if (menu.getState() != Menu::State::PAUSE) {
			roundTimer -= deltaSec;
			if (roundTimer < 0.01f)
				menu.setState(Menu::State::END);

			simulator.stepPhysics(fpsLimit);
			simulator.checkVehiclesOverTile(*arena, vehicles);
		}

		devUI.update(deltaSec, roundTimer);

		// set camera to player vehicles position
		if (!controller->isCameraManual())
		{
			// grab position from player vehicle
			camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getForward());
		}

		// render the updated position of all models and ImGui
		Renderer::getInstance().render(renderables, devUI, menu, camera);

		glfwPollEvents();
	}
	audioPlayer->stopGameMusic();
	audioPlayer->stopCarIdle();
	return;
}

//A loop for endgame
void Engine::endGame()
{
	while (!controller->isWindowClosed()) {
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

		devUI.update(deltaSec, roundTimer);
		controller->processInput(deltaSec);	// will update the menu state once ENTER is pressed.

		// render only the menu for now.
		Renderer::getInstance().render(renderables, devUI, menu, camera);

		glfwPollEvents();
	}
}