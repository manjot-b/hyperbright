#include "Controller.h"

#include <iostream>

Controller::Controller(GLFWwindow* _window, Camera& _camera, std::shared_ptr<Vehicle>& _playerVehicle) :
	window(_window), camera(_camera), playerVehicle(_playerVehicle), isCursorShowing(false), manualCamera(false)

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
	float rotationSpeed = glm::radians(135.0f) * deltaSec;
	float scaleSpeed = 1.0f + 1.0f * deltaSec;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboard(Camera::Movement::RIGHT, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboard(Camera::Movement::LEFT, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyboard(Camera::Movement::UP, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboard(Camera::Movement::DOWN, deltaSec);
	}

	//playerVehicle->resetControls();
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (!upPressed)
		{
			std::cout << "Up key PRESSED" << std::endl;
			playerVehicle->accelerateForward();
			upPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		if (upPressed)
		{
			std::cout << "Up key RELEASED" << std::endl;
			playerVehicle->stopForward();
			upPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (!downPressed)
		{
			std::cout << "Down key PRESSED" << std::endl;
			playerVehicle->accelerateReverse();
			downPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		if (downPressed)
		{
			std::cout << "Down key RELEASED" << std::endl;
			playerVehicle->stopReverse();
			downPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (!leftPressed)
		{
			std::cout << "Left key PRESSED" << std::endl;
			playerVehicle->turnLeft();
			leftPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
	{
		if (leftPressed)
		{
			std::cout << "Left key RELEASED" << std::endl;
			playerVehicle->stopLeft();
			leftPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (!rightPressed)
		{
			std::cout << "Right key PRESSED" << std::endl;
			playerVehicle->turnRight();
			rightPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
	{
		if (rightPressed)
		{
			std::cout << "Right key RELEASED" << std::endl;
			playerVehicle->stopRight();
			rightPressed = false;
		}
	}
}

void Controller::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			controller->setWindowShouldClose(true);
			break;

		case GLFW_KEY_SPACE:
			if (mods & GLFW_MOD_CONTROL)
			{
				controller->toggleCursor();
			}
			break;
		case GLFW_KEY_C:
			controller->manualCamera = !controller->manualCamera;
			std::cout << "Switch to manual camer." << std::endl;
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
		controller->camera.processMouseScroll(Camera::Movement::FORWARD, yoffset);
	if (yoffset < 0)
		controller->camera.processMouseScroll(Camera::Movement::BACKWARD, -yoffset);
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
