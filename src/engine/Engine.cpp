#include "Engine.h"

#include <iostream>

#include "ai/AiManager.h"
#include "TeamStats.h"

namespace hyperbright {
namespace engine {
Engine::Engine() :
	camera(), menu(), devUI(render::Renderer::getInstance().getWindow()),
	deltaSec(0.0f), lastFrame(0.0f), roundTimer(60)
{
	// load textures into a shared pointer.
	loadTextures();
	initEntities();
	setupAudioPlayer();
	pickupManager = std::shared_ptr<entity::PickupManager> (new entity::PickupManager);
	controller = std::make_unique<Controller>(render::Renderer::getInstance().getWindow(), camera, vehicles[0], menu);
}


Engine::~Engine() {}

void Engine::setupAudioPlayer() {
	audioPlayer = std::shared_ptr<audio::AudioPlayer>(new audio::AudioPlayer);
}

void Engine::loadTextures()
{
	face = std::make_shared<openGLHelper::Texture>("rsc/images/awesomeface.png");
	tree = std::make_shared<openGLHelper::Texture>("rsc/images/tree.jpeg");
	background = std::make_shared<openGLHelper::Texture>("rsc/images/background.jpg");
}

//STARTING POSITIONS
glm::vec2 playerStartingPosition;
glm::vec2 ai1StartingPosition;
glm::vec2 ai2StartingPosition;
glm::vec2 ai3StartingPosition;
void Engine::buildArena1 () {
	int arena_size = 40;
	arena = std::make_shared<entity::Arena>(arena_size, arena_size);

	// BUILD ARENA LAYOUT ///////////////////////
	int wallLength = 12;
	int wallWidth = 4;
	arena->addWall(25, 5, wallLength, wallWidth);
	arena->addWall(32, 5, wallLength, wallWidth);

	arena->addWall(5, 5, wallWidth, wallLength);
	arena->addWall(5, 12, wallWidth, wallLength);

	arena->addWall(5, 24, wallLength, wallWidth);
	arena->addWall(12, 24, wallLength, wallWidth);

	arena->addWall(24, 25, wallWidth, wallLength);
	arena->addWall(24, 32, wallWidth, wallLength);

	arena->addWall(0, 0, 1, arena_size); //top
	arena->addWall(0, arena_size - 1, 1, arena_size);//bottom

	arena->addWall(0, 1, arena_size - 1, 1);//left
	arena->addWall(arena_size - 1, 0, arena_size - 1, 1);//right
	////////////////////////////////////////////

	// Starting positions ////////////////////// 
	playerStartingPosition = glm::vec2(18, 18);
	ai1StartingPosition = glm::vec2(18, 24);
	ai2StartingPosition = glm::vec2(24, 18);
	ai3StartingPosition = glm::vec2(24, 24);
	////////////////////////////////////////////
}


void Engine::initEntities()
{	
	buildArena1();//WE CAN HAVE DIFFERENT FUNCTIONS FOR DIFFERENT ARENA BUILDS
	pickupManager->setUp(arena);
	renderables.push_back(arena);

	// Create the player vehicle, setting its starting position, direction, and team (which includes the color of the vehicle/tiles)
	std::shared_ptr<entity::Vehicle> player = std::make_shared<entity::Vehicle>("player", teamStats::Teams::TEAM0, arena->getTilePos(playerStartingPosition) + glm::vec3(0, 1.f ,0), glm::vec3(1.f, 0.f, 0.f));
	vehicles.push_back(player);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(player));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(player));
	
	// Create the 4 ai vehicles, setting their starting position, direction, and team (which includes the color of the vehicle/tiles)
	
	std::shared_ptr<entity::Vehicle> ai1 = std::make_shared<entity::Vehicle>("ai1", teamStats::Teams::TEAM1, arena->getTilePos(ai1StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai1);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(ai1));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai1));

	std::shared_ptr<entity::Vehicle> ai2 = std::make_shared<entity::Vehicle>("ai2", teamStats::Teams::TEAM2, arena->getTilePos(ai2StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai2);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(ai2));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai2));

	std::shared_ptr<entity::Vehicle> ai3 = std::make_shared<entity::Vehicle>("ai3", teamStats::Teams::TEAM3, arena->getTilePos(ai3StartingPosition) + glm::vec3(0, 1.f, 0), glm::vec3(0.f, 0.f, -1.f));
	vehicles.push_back(ai3);
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(ai3));
	physicsModels.push_back(std::static_pointer_cast<physics::IPhysical>(ai3));

	battery = std::make_shared<model::Model> ("rsc/models/cube.obj", "battery", nullptr);
	battery->scale(glm::vec3(1.f, 3.f, 1.f));
	battery->translate(glm::vec3(15.f, 1.5f, 5.f));
	battery->update();
	renderables.push_back(battery);

	// Tempory pickup. Most likely want the pickup manager to handle creation and destruction.
	pickup = std::make_shared<entity::Pickup>();
	renderables.push_back(std::static_pointer_cast<render::Renderer::IRenderable>(pickup));
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
	while (menu.getState() == ui::Menu::State::MAIN) {
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
		render::Renderer::getInstance().render(renderables, devUI, menu, camera);

		glfwPollEvents();
	}
	audioPlayer->stopStartMenuMusic();
}
//////////////////////////////////////////////////////////

void Engine::runGame() {
	physics::Simulate simulator(physicsModels, vehicles, *arena);
	simulator.setAudioPlayer(audioPlayer);

	ai::AiManager aiManager;
	//aiManager.setArena(arena->getAiArenaRepresentation());
	aiManager.setArena(arena);
	aiManager.loadAiVehicle(vehicles.at(1));//MUST LOAD EACH VEHICLE CONTROLLED BY AI
	aiManager.loadAiVehicle(vehicles.at(2));
	aiManager.loadAiVehicle(vehicles.at(3));

	audioPlayer->playGameMusic();
	audioPlayer->playCarIdle();

	while (!controller->isWindowClosed() && menu.getState() != ui::Menu::State::END) {
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
		if (menu.getState() != ui::Menu::State::PAUSE) {
			roundTimer -= deltaSec;
			if (roundTimer < 0.01f)
				menu.setState(ui::Menu::State::END);

			simulator.stepPhysics(fpsLimit);
			simulator.checkVehiclesOverTile(*arena, vehicles);

			pickup->animate(fpsLimit);
		}

		devUI.update(deltaSec, roundTimer);

		// set camera to player vehicles position
		if (!controller->isCameraManual())
		{
			// grab position from player vehicle
			camera.updateCameraVectors(vehicles[0]->getPosition(), vehicles[0]->getForward());
		}

		// render the updated position of all models and ImGui
		render::Renderer::getInstance().render(renderables, devUI, menu, camera);

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
		render::Renderer::getInstance().render(renderables, devUI, menu, camera);

		glfwPollEvents();
	}
}
}	// namespace engine
}	// namespace hyperbright