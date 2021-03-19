#include "Controller.h"

#include <iostream>

namespace hyperbright {
namespace engine {
Controller::Controller(GLFWwindow* _window, render::Camera& _camera, std::shared_ptr<entity::Vehicle>& _playerVehicle, ui::MainMenu& _mainmenu, ui::PauseMenu& _pausemenu, ui::EndMenu& _endmenu) :
	window(_window), camera(_camera), playerVehicle(_playerVehicle), mainmenu(_mainmenu), pausemenu(_pausemenu), endmenu(_endmenu),
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
	if (mainmenu.getState() == ui::MainMenu::State::ON || pausemenu.getState() == ui::PauseMenu::State::ON || endmenu.getState() == ui::EndMenu::State::ON) {
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

	if (controller->mainmenu.getState() == ui::MainMenu::State::ON) {
		controller->mainMenuKeyCallback(key, scancode, action, mods);
	}
	else if (controller->pausemenu.getState() == ui::PauseMenu::State::ON) {
		controller->pauseMenuKeyCallback(key, scancode, action, mods);
	}
	else if (controller->endmenu.getState() == ui::EndMenu::State::ON) {
		controller->endMenuKeyCallback(key, scancode, action, mods);
	}
	else {
		controller->noMenuKeyCallback(key, scancode, action, mods);
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
			mainmenu.setState(ui::MainMenu::State::OFF);
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
		case GLFW_KEY_DOWN:
		{
			ui::PauseMenu::Selection selection = pausemenu.getSelection();
			pausemenu.setSelection(
				selection == ui::PauseMenu::Selection::RESUME ? ui::PauseMenu::Selection::QUIT : ui::PauseMenu::Selection::RESUME);
		}
		break;

		case GLFW_KEY_ENTER:
			if (pausemenu.getSelection() == ui::PauseMenu::Selection::QUIT) {
				setWindowShouldClose(true);
				//controller->menu.setSelection(Menu::Selection::RESUME);
				//controller->menu.setState(Menu::State::MAIN);
			}
			else {
				pausemenu.setState(ui::PauseMenu::State::OFF);
			}
			break;
		}
	}
}
void Controller::noMenuKeyCallback(int key, int scancode, int action, int mods)
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
			pausemenu.setState(ui::PauseMenu::State::ON);
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
		case GLFW_KEY_ENTER:
			setWindowShouldClose(true);
			//mainmenu.setState(ui::MainMenu::State::ON);
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