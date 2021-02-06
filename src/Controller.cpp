#include "Controller.h"

#include <iostream>

Controller::Controller(GLFWwindow* _window, Camera& _camera) :
	window(_window), camera(_camera), modelIndex(0), isCursorShowing(false), manualCamera(false)

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
/*
std::queue<int> Controller::gameInput() {
	//User input
	std::cout << "Queue count: " << currentDrivingControls.size() << std::endl;
	std::queue<int> copy;
	currentDrivingControls.swap(copy);
	return copy;
}
*/
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

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		//std::cout << "Up key PRESSED" << std::endl;
		if (!upPressed)
		{
			//currentDrivingControls.push(ACCEL);
			upPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		if (upPressed)
		{
			std::cout << "Up key RELEASED" << std::endl;
			upPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (!downPressed)
		{
			std::cout << "Down key PRESSED" << std::endl;
			//currentDrivingControls.push(BRAKE);
			downPressed = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		if (downPressed)
		{
			std::cout << "Down key RELEASED" << std::endl;
			downPressed = false;
		}
	}
	/*
	if (!upPressed && !downPressed)
	{
		currentDrivingControls.push(NO_ACC);
	}
	*/
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (!leftPressed)
		{
			std::cout << "Left key PRESSED" << std::endl;
			leftPressed = true;
		}
		//currentDrivingControls.push(TURN_LEFT);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
	{
		if (leftPressed)
		{
			std::cout << "Left key RELEASED" << std::endl;
			leftPressed = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (!rightPressed)
		{
			std::cout << "Right key PRESSED" << std::endl;
			rightPressed = true;
		}
		//currentDrivingControls.push(TURN_RIGHT);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
	{
		if (rightPressed)
		{
			std::cout << "Right key RELEASED" << std::endl;
			rightPressed = false;
		}
	}
	output[0] = upPressed;
	output[1] = downPressed;
	output[2] = leftPressed;
	output[3] = rightPressed;
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
		case GLFW_KEY_N:
			controller->nextModel();
			std::cout << "Next Model. Index: " << controller->modelIndex << std::endl;
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

void Controller::nextModel()
{
	if (modelIndex == numModels)
	{
		modelIndex = 0;
	}
	else
	{
		modelIndex++;
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
