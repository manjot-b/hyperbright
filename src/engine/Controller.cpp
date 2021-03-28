#include "Controller.h"

#include <iostream>

namespace hyperbright {
namespace engine {
Controller::Controller(GLFWwindow* _window,
	render::Camera& _camera,
	ui::MainMenu& _mainmenu,
	ui::PauseMenu& _pausemenu,
	ui::EndMenu& _endmenu,
	audio::AudioPlayer& _audioPlayer) :
	window(_window), camera(_camera),
	mainMenu(_mainmenu), pauseMenu(_pausemenu), endMenu(_endmenu),
	audioPlayer(_audioPlayer), isCursorShowing(false), manualCamera(false)
{
	// The following calls require the Renderer to setup GLFW/glad first.
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Let GLFW store pointer to this instance of Engine.
	glfwSetWindowUserPointer(window, static_cast<void*>(this));
}

void Controller::setPlayerVehicle(std::shared_ptr<entity::Vehicle>& vehicle) { playerVehicle = vehicle; }

Controller::~Controller() {

}

bool wPressed;
bool sPressed;
bool aPressed;
bool dPressed;
bool shiftPressed;
float flipTimer = glfwGetTime();
bool boost;
bool trap;

void Controller::processInput(float deltaSec)
{
	if (mainMenu.getState() != ui::MainMenu::State::OFF || pauseMenu.getState() == ui::PauseMenu::State::ON || endMenu.getState() == ui::EndMenu::State::ON) {
		return;
	}

	/// temp testing
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (!boost) {
			//std::cout << "Up key PRESSED" << std::endl;
			playerVehicle->applyBoost(200);
			boost = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
	{
		if (boost) {
			//std::cout << "Up key PRESSED" << std::endl;
			playerVehicle->releaseBoost();
			boost = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		if (!trap) {
			//std::cout << "Up key PRESSED" << std::endl;
			playerVehicle->applyTrap(200);
			trap = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		if (trap) {
			//std::cout << "Up key PRESSED" << std::endl;
			//playerVehicle->releaseTrap();
			trap = false;
		}
	}


	////////////////////////////////////////// PLAYER VEHICLE CONTROLS

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (!wPressed) {
			//std::cout << "Up key PRESSED" << std::endl;
			playerVehicle->accelerateForward();
			wPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
	{
		if (wPressed)
		{
			//std::cout << "Up key RELEASED" << std::endl;
			playerVehicle->stopForward();
			wPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (!aPressed) {
			//std::cout << "Left key PRESSED" << std::endl;
			playerVehicle->turnLeft();
			aPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
	{
		if (aPressed)
		{
			//std::cout << "Left key RELEASED" << std::endl;
			playerVehicle->stopLeft();
			aPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		// *** need to run accel reverse every frame to check if braking or reversing
		//std::cout << "Down key PRESSED" << std::endl;
		playerVehicle->accelerateReverse();
		sPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
	{
		if (sPressed)
		{
			//std::cout << "Down key RELEASED" << std::endl;
			playerVehicle->stopReverse();
			sPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		
		if(!dPressed) {
			//std::cout << "Right key PRESSED" << std::endl;
			playerVehicle->turnRight();
			dPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
	{
		if (dPressed)
		{
			//std::cout << "Right key RELEASED" << std::endl;
			playerVehicle->stopRight();
			dPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		if (!shiftPressed)
		{
			playerVehicle->hardTurn();
			shiftPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		if (shiftPressed)
		{
			playerVehicle->stopHardTurn();
			shiftPressed = false;
		}
	}

	/////////////// FLIP VEHICLE CONTROLS
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		// only apply the flip impulse if the vehicle is upside down and 5 seconds has past since the last attempt
		if (!playerVehicle->isUpright() && (glfwGetTime() - flipTimer > 2.f)) 
		{
			playerVehicle->applyFlipImpulse();
			flipTimer = glfwGetTime();
		}
	}

	/////////////// VEHICLE PICKUP ACTIVATE CONTROLS
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (playerVehicle->hasPickup()) {
			int type = playerVehicle->getPickup()->type;
			if (type == EMP) {
				audioPlayer.playEmpSound();
			}
			else if (type == SPEED) {
				audioPlayer.playSpeedSound();
			}
			else {
				audioPlayer.playUsePowerupSound();
			}
			playerVehicle->activatePickup();		
		}
	}

	////////////////////////////////////////// MANUAL CAMERA CONTROLS

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (manualCamera) {
			camera.processKeyboard(render::Camera::Movement::UP, deltaSec);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (manualCamera) {
			camera.processKeyboard(render::Camera::Movement::DOWN, deltaSec);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (manualCamera) {
			camera.processKeyboard(render::Camera::Movement::LEFT, deltaSec);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (manualCamera) {
			camera.processKeyboard(render::Camera::Movement::RIGHT, deltaSec);
		}
	}
}

void Controller::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (controller->mainMenu.getState() != ui::MainMenu::State::OFF) {
		controller->mainMenuKeyCallback(key, scancode, action, mods);
	}
	else if (controller->pauseMenu.getState() == ui::PauseMenu::State::ON) {
		controller->pauseMenuKeyCallback(key, scancode, action, mods);
	}
	else if (controller->endMenu.getState() == ui::EndMenu::State::ON) {
		controller->endMenuKeyCallback(key, scancode, action, mods);
	}
	else {
		controller->gameKeyCallback(key, scancode, action, mods);
	}

	// Be able to access the DevUI window from any menu
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_SPACE:
			if (mods & GLFW_MOD_CONTROL)
			{
				controller->toggleCursor();
			}
			break;
		}
	}
}

void Controller::mainMenuKeyCallback(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ENTER:
			if (mainMenu.getState() == ui::MainMenu::State::WELCOME) {
				mainMenu.setState(ui::MainMenu::State::SETUP);
			}
			else {	// Finished SETUP. Enter game.
				mainMenu.setState(ui::MainMenu::State::OFF);
			}
			audioPlayer.playMenuEnterSound();
			break;
		case GLFW_KEY_RIGHT:
		case GLFW_KEY_LEFT:
			if (mainMenu.getState() == ui::MainMenu::State::SETUP) {
				mainMenu.setArenaSelection(mainMenu.getArenaSelection() == ui::MainMenu::ArenaSelection::ARENA1 ? 
					ui::MainMenu::ArenaSelection::ARENA2 : ui::MainMenu::ArenaSelection::ARENA1);
			}
			break;
		}
	}
}

void Controller::pauseMenuKeyCallback(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key) {
		case GLFW_KEY_UP:
		{
			ui::PauseMenu::Selection selection = pauseMenu.getSelection();
			int count = static_cast<int>(ui::PauseMenu::Selection::LAST);
			int nextIdx = static_cast<int>(selection) - 1;

			// modulo in c++ is not equivalent to mathematical modulo operation when dealing with negative numbers.
			nextIdx = (count + (nextIdx % count)) % count;
			ui::PauseMenu::Selection nextSelection = static_cast<ui::PauseMenu::Selection>(nextIdx);

			pauseMenu.setSelection(nextSelection);
			audioPlayer.playMenuSwitchSound();
		}
		break;
		case GLFW_KEY_DOWN:
		{
			ui::PauseMenu::Selection selection = pauseMenu.getSelection();
			unsigned int count = static_cast<unsigned int>(ui::PauseMenu::Selection::LAST);
			unsigned int nextIdx = (static_cast<unsigned int>(selection) + 1) % count;
			ui::PauseMenu::Selection nextSelection = static_cast<ui::PauseMenu::Selection>(nextIdx);

			pauseMenu.setSelection(nextSelection);
			audioPlayer.playMenuSwitchSound();
		}
		break;

		case GLFW_KEY_ENTER:
			if (pauseMenu.getSelection() == ui::PauseMenu::Selection::QUIT) {
				setWindowShouldClose(true);
			}
			else if (pauseMenu.getSelection() == ui::PauseMenu::Selection::MAIN_MENU) {
				pauseMenu.setSelection(ui::PauseMenu::Selection::RESUME);
				pauseMenu.setState(ui::PauseMenu::State::OFF);
				mainMenu.setState(ui::MainMenu::State::WELCOME);
			}
			else {
				pauseMenu.setState(ui::PauseMenu::State::OFF);
			}
			audioPlayer.playMenuEnterSound();
			break;
		}
	}
}
void Controller::gameKeyCallback(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_C:
			manualCamera = !manualCamera;
			std::cout << "Switch to manual camera." << std::endl;
			break;

		case GLFW_KEY_ESCAPE:
			pauseMenu.setState(ui::PauseMenu::State::ON);
			break;
		}
	}
}

void Controller::endMenuKeyCallback(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
		case GLFW_KEY_DOWN:
		{
			ui::EndMenu::Selection selection = endMenu.getSelection();
			endMenu.setSelection(
				selection == ui::EndMenu::Selection::MAIN_MENU ? ui::EndMenu::Selection::QUIT : ui::EndMenu::Selection::MAIN_MENU);
			audioPlayer.playMenuSwitchSound();
			break;
		}
		case GLFW_KEY_ENTER:
			if (endMenu.getSelection() == ui::EndMenu::Selection::MAIN_MENU) {
				endMenu.setState(ui::EndMenu::State::OFF);
				mainMenu.setState(ui::MainMenu::State::WELCOME);
			}
			else {
				setWindowShouldClose(true);
			}
			audioPlayer.playMenuEnterSound();
			break;
		}
	}
}

void Controller::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (!controller->isCursorShowing && controller->manualCamera &&
		controller->pauseMenu.getState() == ui::PauseMenu::State::OFF &&
		controller->endMenu.getState() == ui::EndMenu::State::OFF)
	{
		if (controller->firstMouse)
		{
			controller->lastX = xpos;
			controller->lastY = ypos;
			controller->firstMouse = false;
		}

		float xoffset = xpos - controller->lastX;
		float yoffset = controller->lastY - ypos; // reversed since y-coordinates go from bottom to top

		controller->lastX = xpos;
		controller->lastY = ypos;

		controller->camera.processMouseMovement(xoffset, yoffset);
	}
}

void Controller::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (!controller->isCursorShowing && controller->manualCamera &&
		controller->pauseMenu.getState() == ui::PauseMenu::State::OFF &&
		controller->endMenu.getState() == ui::EndMenu::State::OFF)
	{
		if (yoffset > 0)
			controller->camera.processMouseScroll(render::Camera::Movement::FORWARD, yoffset);
		if (yoffset < 0)
			controller->camera.processMouseScroll(render::Camera::Movement::BACKWARD, -yoffset);
	}
}

void Controller::toggleCursor()
{
	isCursorShowing = !isCursorShowing;
	int cursorMode = isCursorShowing ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
	glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
}

void Controller::setWindowShouldClose(bool close)
{
	glfwSetWindowShouldClose(window, close);
}

bool Controller::isWindowClosed() const
{
	return glfwWindowShouldClose(window);
}
}	// namespace engine
}	// namespace hyperbright