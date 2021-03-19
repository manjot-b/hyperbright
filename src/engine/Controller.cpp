#include "Controller.h"

#include <iostream>

namespace hyperbright {
namespace engine {
Controller::Controller(GLFWwindow* _window, render::Camera& _camera, std::shared_ptr<entity::Vehicle>& _playerVehicle, ui::MainMenu& _mainmenu, ui::PauseMenu& _pausemenu, ui::EndMenu& _endmenu) :
	window(_window), camera(_camera), playerVehicle(_playerVehicle), mainMenu(_mainmenu), pauseMenu(_pausemenu), endMenu(_endmenu),
	isCursorShowing(false), manualCamera(false)
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

Controller::~Controller() {

}

bool upPressed;
bool downPressed;
bool leftPressed;
bool rightPressed;

void Controller::processInput(float deltaSec)
{
	if (mainMenu.getState() == ui::MainMenu::State::ON || pauseMenu.getState() == ui::PauseMenu::State::ON || endMenu.getState() == ui::EndMenu::State::ON) {
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboard(render::Camera::Movement::RIGHT, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboard(render::Camera::Movement::LEFT, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyboard(render::Camera::Movement::UP, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboard(render::Camera::Movement::DOWN, deltaSec);
	}

	//playerVehicle->resetControls();
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (!upPressed)
		{
			//std::cout << "Up key PRESSED" << std::endl;
			playerVehicle->accelerateForward();
			upPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		if (upPressed)
		{
			//std::cout << "Up key RELEASED" << std::endl;
			playerVehicle->stopForward();
			upPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (!downPressed)
		{
			//std::cout << "Down key PRESSED" << std::endl;
			playerVehicle->accelerateReverse();
			downPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		if (downPressed)
		{
			//std::cout << "Down key RELEASED" << std::endl;
			playerVehicle->stopReverse();
			downPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (!leftPressed)
		{
			//std::cout << "Left key PRESSED" << std::endl;
			playerVehicle->turnLeft();
			leftPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
	{
		if (leftPressed)
		{
			//std::cout << "Left key RELEASED" << std::endl;
			playerVehicle->stopLeft();
			leftPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (!rightPressed)
		{
			//std::cout << "Right key PRESSED" << std::endl;
			playerVehicle->turnRight();
			rightPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
	{
		if (rightPressed)
		{
			//std::cout << "Right key RELEASED" << std::endl;
			playerVehicle->stopRight();
			rightPressed = false;
		}
	}
}

void Controller::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (controller->mainMenu.getState() == ui::MainMenu::State::ON) {
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
			mainMenu.setState(ui::MainMenu::State::OFF);
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
		}
		break;
		case GLFW_KEY_DOWN:
		{
			ui::PauseMenu::Selection selection = pauseMenu.getSelection();
			unsigned int count = static_cast<unsigned int>(ui::PauseMenu::Selection::LAST);
			unsigned int nextIdx = (static_cast<unsigned int>(selection) + 1) % count;
			ui::PauseMenu::Selection nextSelection = static_cast<ui::PauseMenu::Selection>(nextIdx);

			pauseMenu.setSelection(nextSelection);
		}
		break;

		case GLFW_KEY_ENTER:
			if (pauseMenu.getSelection() == ui::PauseMenu::Selection::QUIT) {
				setWindowShouldClose(true);
			}
			else if (pauseMenu.getSelection() == ui::PauseMenu::Selection::MAIN_MENU) {
				pauseMenu.setSelection(ui::PauseMenu::Selection::RESUME);
				pauseMenu.setState(ui::PauseMenu::State::OFF);
				mainMenu.setState(ui::MainMenu::State::ON);
			}
			else {
				pauseMenu.setState(ui::PauseMenu::State::OFF);
			}
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
			break;
		}
		case GLFW_KEY_ENTER:
			if (endMenu.getSelection() == ui::EndMenu::Selection::MAIN_MENU) {
				endMenu.setState(ui::EndMenu::State::OFF);
				mainMenu.setState(ui::MainMenu::State::ON);
			}
			else {
				setWindowShouldClose(true);
			}
			
			break;
		}
	}
}

void Controller::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (!controller->isCursorShowing && controller->manualCamera)
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

	if (yoffset > 0)
		controller->camera.processMouseScroll(render::Camera::Movement::FORWARD, yoffset);
	if (yoffset < 0)
		controller->camera.processMouseScroll(render::Camera::Movement::BACKWARD, -yoffset);
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