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

bool forward;
bool backward;
bool left;
bool right;
bool handbrake;
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
	
	////////////////////////////////////////// JOYSTICK

	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
		int count;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

		if (GLFW_PRESS == buttons[0]) { // square
			std::cout << "Button 0" << std::endl;
		}

		if (GLFW_RELEASE == buttons[0]) {

		}

		if (GLFW_PRESS == buttons[1]) { // cross
			std::cout << "Button 1" << std::endl;
			if (!forward) {
				//std::cout << "Up key PRESSED" << std::endl;
				playerVehicle->accelerateForward();
				forward = true;
			}
		}

		if (GLFW_RELEASE == buttons[1]) {
			if (forward)
			{
				//std::cout << "Up key RELEASED" << std::endl;
				playerVehicle->stopForward();
				forward = false;
			}
		}

		if (GLFW_PRESS == buttons[2]) { // circle
			std::cout << "Button 2" << std::endl;
		}

		if (GLFW_PRESS == buttons[3]) { // triangle
			std::cout << "Button 3" << std::endl;
		}

		if (GLFW_PRESS == buttons[4]) { // L1
			std::cout << "Button 4" << std::endl;
		}

		if (GLFW_PRESS == buttons[5]) { // R1
			std::cout << "Button 5" << std::endl;
		}

		if (GLFW_PRESS == buttons[6]) { // L2
			std::cout << "Button 6" << std::endl;
		}

		if (GLFW_PRESS == buttons[7]) { // R2
			std::cout << "Button 7" << std::endl;
		}

		if (GLFW_PRESS == buttons[8]) { // share
			std::cout << "Button 8" << std::endl;
		}

		if (GLFW_PRESS == buttons[9]) { // options
			std::cout << "Button 9" << std::endl;
		}

		if (GLFW_PRESS == buttons[10]) { // L3
			std::cout << "Button 10" << std::endl;
		}

		if (GLFW_PRESS == buttons[11]) { // R3
			std::cout << "Button 11" << std::endl;
		}

		if (GLFW_PRESS == buttons[12]) { // PS
			std::cout << "Button 12" << std::endl;
		}

		if (GLFW_PRESS == buttons[13]) { // touch pad
			std::cout << "Button 13" << std::endl;
		}
	}

	////////////////////////////////////////// KEYBOARD
	if (!glfwJoystickPresent(GLFW_JOYSTICK_1)) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (!forward) {
				//std::cout << "Up key PRESSED" << std::endl;
				playerVehicle->accelerateForward();
				forward = true;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
		{
			if (forward)
			{
				//std::cout << "Up key RELEASED" << std::endl;
				playerVehicle->stopForward();
				forward = false;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			if (!left) {
				//std::cout << "Left key PRESSED" << std::endl;
				playerVehicle->turnLeft();
				left = true;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
		{
			if (left)
			{
				//std::cout << "Left key RELEASED" << std::endl;
				playerVehicle->stopLeft();
				left = false;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			// *** need to run accel reverse every frame to check if braking or reversing
			//std::cout << "Down key PRESSED" << std::endl;
			playerVehicle->accelerateReverse();
			backward = true;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
		{
			if (backward)
			{
				//std::cout << "Down key RELEASED" << std::endl;
				playerVehicle->stopReverse();
				backward = false;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{

			if (!right) {
				//std::cout << "Right key PRESSED" << std::endl;
				playerVehicle->turnRight();
				right = true;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
		{
			if (right)
			{
				//std::cout << "Right key RELEASED" << std::endl;
				playerVehicle->stopRight();
				right = false;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			if (!handbrake)
			{
				playerVehicle->hardTurn();
				handbrake = true;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			if (handbrake)
			{
				playerVehicle->stopHardTurn();
				handbrake = false;
			}
		}

		/////////////// FLIP VEHICLE CONTROLS
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			// only apply the flip impulse if the vehicle is upside down and 1 second has past since the last attempt
			if (!playerVehicle->isUpright() && (glfwGetTime() - flipTimer > 1.f))
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
		{
			ui::MainMenu::ArenaSelection selection = mainMenu.getArenaSelection();
			int count = static_cast<int>(ui::MainMenu::ArenaSelection::LAST);
			int nextIdx = static_cast<int>(selection) + 1;

			nextIdx = nextIdx % count;
			ui::MainMenu::ArenaSelection nextSelection = static_cast<ui::MainMenu::ArenaSelection>(nextIdx);

			mainMenu.setArenaSelection(nextSelection);
			audioPlayer.playMenuSwitchSound();
		}
		break;
		case GLFW_KEY_LEFT:
		{
			ui::MainMenu::ArenaSelection selection = mainMenu.getArenaSelection();
			int count = static_cast<int>(ui::MainMenu::ArenaSelection::LAST);
			int nextIdx = static_cast<int>(selection) - 1;

			// modulo in c++ is not equivalent to mathematical modulo operation when dealing with negative numbers.
			nextIdx = (count + (nextIdx % count)) % count;
			ui::MainMenu::ArenaSelection nextSelection = static_cast<ui::MainMenu::ArenaSelection>(nextIdx);

			mainMenu.setArenaSelection(nextSelection);
			audioPlayer.playMenuSwitchSound();
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