#include "Controller.h"
#include "Vehicle.h"
#include "Ai.h"

#include <iostream>

Controller::Controller(GLFWwindow* _window, std::shared_ptr<Camera> _camera) :
	window(_window), camera(_camera), modelIndex(0), isCursorShowing(false)
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

void Controller::gameInput(Vehicle vehicle) {
	//User input
}

void Controller::processInput(float deltaSec)
{
	float rotationSpeed = glm::radians(135.0f) * deltaSec;
	float scaleSpeed = 1.0f + 1.0f * deltaSec;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->processKeyboard(Camera::Movement::RIGHT, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->processKeyboard(Camera::Movement::LEFT, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->processKeyboard(Camera::Movement::UP, deltaSec);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->processKeyboard(Camera::Movement::DOWN, deltaSec);
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
		case GLFW_KEY_N:
			controller->nextModel();
			std::cout << "Next Model. Index: " << controller->modelIndex << std::endl;
			break;
		}
	}
}

void Controller::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (!controller->isCursorShowing)
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

		controller->camera->processMouseMovement(xoffset, yoffset);
	}
}

void Controller::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));

	if (yoffset > 0)
		controller->camera->processMouseScroll(Camera::Movement::FORWARD, yoffset);
	if (yoffset < 0)
		controller->camera->processMouseScroll(Camera::Movement::BACKWARD, -yoffset);
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
