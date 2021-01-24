#include "Engine.h"
#include "Simulate.h"
#include "Arena.h"
#include "Pickup.h"
#include "Ai.h"
#include "Vehicle.h"
#include "DevUI.h"
#include "Controller.h"

#include <filesystem>

#define STARTGAME 1
#define NOINPUT 0
#define ENDGAME 2


Engine::Engine() :
	deltaSec(0.0f), rotate(0), scale(1),
	lastFrame(0.0f)
{
	camera = std::make_shared<Camera>();
	renderer = std::make_unique<Renderer>(camera);
	loadModels();

	// The following calls require the Renderer to setup GLFW/glad first.
	glfwSetKeyCallback(renderer->getWindow(), keyCallback);
	glfwSetCursorPosCallback(renderer->getWindow(), mouseCallback);
	// tell GLFW to capture our mouse
	glfwSetInputMode(renderer->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Let GLFW store pointer to this instance of Engine.
	glfwSetWindowUserPointer(renderer->getWindow(), static_cast<void*>(this));
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

	//Controller controller;
	//renderer.run();

	while (!renderer->isWindowClosed()) {
		// update global time
		float currentFrame = glfwGetTime();
		deltaSec = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processWindowInput();

		//simulator.stepPhysics();
		// The following code should be in the Simulator.
		for (auto& model : models)
		{
			model->rotate(rotate);
			model->scale(scale);
			model->update();
		}

		renderer->render(deltaSec, devUI, models);

		rotate = glm::vec3(0.0f);
		scale = 1;

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

/*
 * This method typically runs faster than handling a key callback.
 * So controls like movements should be placed in here.
 */
void Engine::processWindowInput()
{
	float rotationSpeed = glm::radians(135.0f) * deltaSec;
	float scaleSpeed = 1.0f + 1.0f * deltaSec;
	shiftPressed = glfwGetKey(renderer->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	// Rotations
	if (!shiftPressed)
	{
		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
		{
			rotate.x -= rotationSpeed;
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		{
			rotate.x += rotationSpeed;
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_E) == GLFW_PRESS)
		{
			rotate.y += rotationSpeed;
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
		{
			rotate.y -= rotationSpeed;
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		{
			rotate.z -= rotationSpeed;
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
		{
			rotate.z += rotationSpeed;
		}
	}

	// Camera Movement
	if (shiftPressed)
	{
		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
		{
			camera->processKeyboard(Camera::Movement::FORWARD, deltaSec);
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		{
			camera->processKeyboard(Camera::Movement::BACKWARD, deltaSec);
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		{
			camera->processKeyboard(Camera::Movement::RIGHT, deltaSec);
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
		{
			camera->processKeyboard(Camera::Movement::LEFT, deltaSec);
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_E) == GLFW_PRESS)
		{
			camera->processKeyboard(Camera::Movement::UP, deltaSec);
		}

		if (glfwGetKey(renderer->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
		{
			camera->processKeyboard(Camera::Movement::DOWN, deltaSec);
		}
	}

	// Scaling
	if (glfwGetKey(renderer->getWindow(), GLFW_KEY_Z) == GLFW_PRESS)
	{
		scale *= scaleSpeed;
	}
	if (glfwGetKey(renderer->getWindow(), GLFW_KEY_X) == GLFW_PRESS)
	{
		scale /= scaleSpeed;
	}
}

/*
 * Handle keyboard inputs that don't require frequent repeated actions,
 * ex closing window, selecting model etc.
 */
void Engine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			engine->renderer->setWindowShouldClose(true);
			break;

		case GLFW_KEY_SPACE:
			if (mods & GLFW_MOD_CONTROL)
			{
				engine->renderer->toggleCursor();
			}
			break;
		}
	}
}

void Engine::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

	if (!engine->renderer->isCursorShowing())
	{
		if (engine->firstMouse)
		{
			engine->lastX = xpos;
			engine->lastY = ypos;
			engine->firstMouse = false;
		}

		float xoffset = xpos - engine->lastX;
		float yoffset = engine->lastY - ypos; // reversed since y-coordinates go from bottom to top

		engine->lastX = xpos;
		engine->lastY = ypos;

		engine->camera->processMouseMovement(xoffset, yoffset);
	}
}