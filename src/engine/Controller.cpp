#include "Controller.h"

#include <iostream>

namespace hyperbright {
namespace engine {

Controller::Controller(GLFWwindow* _window,
	render::Camera& _camera,
	ui::MainMenu& _mainmenu,
	ui::PauseMenu& _pausemenu,
	ui::EndMenu& _endmenu,
	ui::LoadingScreen& _loadingScreen,
	audio::AudioPlayer& _audioPlayer) :
	window(_window), camera(_camera),
	mainMenu(_mainmenu), pauseMenu(_pausemenu), endMenu(_endmenu), loadingScreen(_loadingScreen),
	audioPlayer(_audioPlayer), isCursorShowing(false), manualCamera(false)
{
	// The following calls require the Renderer to setup GLFW/glad first.
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Let GLFW store pointer to this instance of Controller.
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
	if (mainMenu.getState() != ui::MainMenu::State::OFF ||
		loadingScreen.getState() == ui::LoadingScreen::State::WAITING ||
		pauseMenu.getState() == ui::PauseMenu::State::ON ||
		endMenu.getState() == ui::EndMenu::State::ON) {
		if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
			glfwGetGamepadState(GLFW_JOYSTICK_1, &joystick);

			// check state of the game and use appropraite joystick controls
			joystickCallback(window, joystick);
		}
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
		glfwGetGamepadState(GLFW_JOYSTICK_1, &joystick);
		playerVehicle->setAnalogController(true);

		// check state of the game and use appropraite joystick controls
		joystickCallback(window, joystick);

		int count;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

		if (joystick.buttons[GLFW_GAMEPAD_BUTTON_A]) { // ACTIVATE PICKUP
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

		if (joystick.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]) { // HANDBRAKE
			if (!handbrake)
			{
				playerVehicle->hardTurn();
				handbrake = true;
			}
		}
		else {
			if (handbrake)
			{
				playerVehicle->stopHardTurn();
				handbrake = false;
			}
		}

		/*if (joystick.buttons[GLFW_GAMEPAD_BUTTON_X]) { // BOOST TESTING
				if (!boost) {
					//std::cout << "Up key PRESSED" << std::endl;
					playerVehicle->applyBoost(200);
					boost = true;
				}
		}
		else {
			if (boost) {
				//std::cout << "Up key PRESSED" << std::endl;
				playerVehicle->releaseBoost();
				boost = false;
			}
		}*/

		if (joystick.buttons[GLFW_GAMEPAD_BUTTON_Y]) { // FLIP
			// only apply the flip impulse if the vehicle is upside down and 1 second has past since the last attempt
			if (!playerVehicle->isUpright() && (glfwGetTime() - flipTimer > 1.f))
			{
				playerVehicle->applyFlipImpulse();
				flipTimer = glfwGetTime();
			}
		}

		if (joystick.buttons[GLFW_GAMEPAD_BUTTON_START]) { // PAUSE
			if (pauseMenu.getState() == ui::PauseMenu::State::OFF) {
				pauseMenu.setState(ui::PauseMenu::State::ON);
			}
			else {
				pauseMenu.setState(ui::PauseMenu::State::OFF);
			}
		}

		float threshhold = 0.15f;
		const float turn = joystick.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
		if (turn > threshhold) {
			if (left)	// stop turning left
			{
				playerVehicle->stopLeft();
				left = false;
			}
			// turn right
			playerVehicle->turnRight(turn);
			right = true;
		}
		else if (turn < -threshhold) {
			if (right)	// stop turning right
			{
				playerVehicle->stopRight();
				right = false;
			}
			// turn left
			playerVehicle->turnLeft(turn);
			left = true;
		}
		else {	// stop turning both directions
			if (left)	// stop turning left
			{
				playerVehicle->stopLeft();
				left = false;
			}

			if (right)	// stop turning right
			{
				playerVehicle->stopRight();
				right = false;
			}
		}

		if (joystick.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > -0.9f) {
			playerVehicle->accelerateReverse((1.f + joystick.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]) / 2.f);
			backward = true;
		}
		else {
			if (backward)
			{
				playerVehicle->stopReverse();
				backward = false;
			}
		}

		if (joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > -0.9f) {
			playerVehicle->accelerateForward((1.f + joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]) / 2.f);
			forward = true;
		}
		else {
			if (forward)
			{
				playerVehicle->stopForward();
				forward = false;
			}
		}

		///////////////// PAN CAMERA AROUND
		if (joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] > 0.05) {
			camera.panLeft(0.f);
			camera.panRight(joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
		}
		else if (joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] < -0.05) {
			camera.panRight(0.f);
			camera.panLeft(joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
		}
		else {
			camera.panLeft(0.f);
			camera.panRight(0.f);
		}


		float lookBackThreshhold = 0.2f;
		float lookForwardThreshhold = 0.2f;
		if (joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] > lookBackThreshhold) {
			camera.lookBack(true);
		}
		else if (joystick.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] < lookForwardThreshhold) {
			camera.lookBack(false);
		}
	}
	else {
		playerVehicle->setAnalogController(false);
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
			else if (type == ZAP) {
				audioPlayer.playZapSound();
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
	else if (controller->loadingScreen.getState() == ui::LoadingScreen::State::WAITING) {
		controller->loadingKeyCallback(key, scancode, action, mods);
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

	#if _DEBUG
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
	#endif
}

void Controller::joystickCallback(GLFWwindow* window, GLFWgamepadstate& joystick)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (controller->mainMenu.getState() != ui::MainMenu::State::OFF) {
		controller->mainMenuJoystickCallback(joystick);
	}
	else if (controller->loadingScreen.getState() == ui::LoadingScreen::State::WAITING) {
		controller->loadingJoystickCallback(joystick);
	}
	else if (controller->pauseMenu.getState() == ui::PauseMenu::State::ON) {
		controller->pauseMenuJoystickCallback(joystick);
	}
	else if (controller->endMenu.getState() == ui::EndMenu::State::ON) {
		controller->endMenuJoystickCallback(joystick);
	}
}

void Controller::mainMenuKeyCallback(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ENTER:
			mainMenuSelectButton();
			break;
		case GLFW_KEY_UP:
		case GLFW_KEY_DOWN:
			mainMenuUpOrDownButton();
			break;
		case GLFW_KEY_RIGHT:
			mainMenuRightButton();
			break;
		case GLFW_KEY_LEFT:
			mainMenuLeftButton();
			break;
		}
	}
}

bool mainMenuLeft = false;
bool mainMenuRight = false;
bool mainMenuUpOrDown = false;
bool mainMenuEnter = false;

void Controller::mainMenuJoystickCallback(GLFWgamepadstate& joystick)
{
	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] || joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
		if (!mainMenuUpOrDown) {
			mainMenuUpOrDown = true;
			mainMenuUpOrDownButton();
		}
	}
	else { mainMenuUpOrDown= false; }

	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) {
		if (!mainMenuLeft) {
			mainMenuLeft = true;
			mainMenuLeftButton();
		}
	}
	else { mainMenuLeft = false; }

	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) {
		if (!mainMenuRight) {
			mainMenuRight = true;
			mainMenuRightButton();
		}
	}
	else { mainMenuRight = false; }

	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_A]) {
		if (!mainMenuEnter) {
			mainMenuEnter = true;
			std::cout << "Button A" << std::endl;
			mainMenuSelectButton();
		}
	}
	else { mainMenuEnter = false; }
}

void Controller::mainMenuSelectButton()
{
	switch (mainMenu.getSelection())
	{
	case ui::MainMenu::Selection::START:
		if (mainMenu.getState() == ui::MainMenu::State::WELCOME) {
			mainMenu.setState(ui::MainMenu::State::SETUP);
		}
		else {	// Finished SETUP. Enter game.
			mainMenu.setState(ui::MainMenu::State::OFF);
		}
		audioPlayer.playMenuEnterSound();
		break;
	case ui::MainMenu::Selection::EXIT:
		setWindowShouldClose(true);
		break;
	}
}

void Controller::mainMenuRightButton()
{
	if (mainMenu.getState() == ui::MainMenu::State::SETUP)
	{
		ui::MainMenu::ArenaSelection selection = mainMenu.getArenaSelection();
		int count = static_cast<int>(ui::MainMenu::ArenaSelection::LAST);
		int nextIdx = static_cast<int>(selection) + 1;

		nextIdx = nextIdx % count;
		ui::MainMenu::ArenaSelection nextSelection = static_cast<ui::MainMenu::ArenaSelection>(nextIdx);

		mainMenu.setArenaSelection(nextSelection);
		audioPlayer.playMenuSwitchSound();
	}
}

void Controller::mainMenuLeftButton()
{
	if (mainMenu.getState() == ui::MainMenu::State::SETUP)
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
}

void Controller::mainMenuUpOrDownButton()
{
	if (mainMenu.getState() == ui::MainMenu::State::WELCOME)
	{
		mainMenu.setSelection(mainMenu.getSelection() == ui::MainMenu::Selection::START ?
			ui::MainMenu::Selection::EXIT : ui::MainMenu::Selection::START);
		audioPlayer.playMenuSwitchSound();
	}
}

void Controller::pauseMenuKeyCallback(int key, int scancode, int action, int mods)
{
	audioPlayer.stopCarIdle();
	if (action == GLFW_PRESS)
	{
		switch (key) {
		case GLFW_KEY_UP:
			pauseUpButton();
			break;
		case GLFW_KEY_DOWN:
			pauseDownButton();
			break;
		case GLFW_KEY_ENTER:
			pauseSelectButton();
			break;
		}
	}
}

bool pauseMenuUp = false;
bool pauseMenuDown = false;

void Controller::pauseMenuJoystickCallback(GLFWgamepadstate& joystick)
{
	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) {
		if (!pauseMenuUp) {
			pauseMenuUp = true;
			pauseUpButton();
		}
	}
	else { pauseMenuUp = false; }

	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
		if (!pauseMenuDown) {
			pauseMenuDown = true;
			pauseDownButton();
		}
	}
	else { pauseMenuDown = false; }

	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_A]) {
		pauseSelectButton();
	}
}

void Controller::pauseSelectButton()
{
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
		audioPlayer.playCarIdle();
	}
	audioPlayer.playMenuEnterSound();
}

void Controller::pauseUpButton()
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

void Controller::pauseDownButton()
{
	ui::PauseMenu::Selection selection = pauseMenu.getSelection();
	unsigned int count = static_cast<unsigned int>(ui::PauseMenu::Selection::LAST);
	unsigned int nextIdx = (static_cast<unsigned int>(selection) + 1) % count;
	ui::PauseMenu::Selection nextSelection = static_cast<ui::PauseMenu::Selection>(nextIdx);

	pauseMenu.setSelection(nextSelection);
	audioPlayer.playMenuSwitchSound();
}

void Controller::loadingKeyCallback(int key, int scancode, int action, int mods)
{
	// if any key or button is pressed then transition the state.
	if (action == GLFW_PRESS)
		loadingScreen.setState(ui::LoadingScreen::State::DONE);
}

void Controller::loadingJoystickCallback(GLFWgamepadstate& joystick)
{
	// if any key or button is pressed then transition the state.

	for (unsigned int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; i++)
	{
		if (joystick.buttons[i])
		{
			loadingScreen.setState(ui::LoadingScreen::State::DONE);
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

void Controller::gameJoystickCallback(GLFWgamepadstate& joystick)
{
	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_START]) {
		pauseMenu.setState(ui::PauseMenu::State::ON);
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
			endUpOrDownButton();
			break;
		case GLFW_KEY_ENTER:
			endSelectButton();
			break;
		}
	}
}

bool endMenuChangeSelected = false;

void Controller::endMenuJoystickCallback(GLFWgamepadstate& joystick)
{
	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] || joystick.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
		if (!endMenuChangeSelected) {
			endMenuChangeSelected = true;
			endUpOrDownButton();
		}
	}
	else { endMenuChangeSelected = false; }

	if (joystick.buttons[GLFW_GAMEPAD_BUTTON_A]) {
		endSelectButton();
	}
}

void Controller::endSelectButton()
{
	if (endMenu.getSelection() == ui::EndMenu::Selection::MAIN_MENU) {
		endMenu.setState(ui::EndMenu::State::OFF);
		mainMenu.setState(ui::MainMenu::State::WELCOME);
	}
	else {
		setWindowShouldClose(true);
	}
	audioPlayer.playMenuEnterSound();
}

void Controller::endUpOrDownButton()
{
	ui::EndMenu::Selection selection = endMenu.getSelection();
	endMenu.setSelection(
		selection == ui::EndMenu::Selection::MAIN_MENU ? ui::EndMenu::Selection::QUIT : ui::EndMenu::Selection::MAIN_MENU);
	audioPlayer.playMenuSwitchSound();
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