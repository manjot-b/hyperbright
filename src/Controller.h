#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <queue>

#include "Vehicle.h"
#include "Camera.h"

class Controller
{
public:

	enum Drive {
		ACCEL,
		BRAKE,
		TURN_LEFT,
		TURN_LEFT_HARD,
		LEFT_RELEASED,
		TURN_RIGHT,
		TURN_RIGHT_HARD,
		RIGHT_RELEASED,
		NO_ACC,
		NO_TURN
	};

	Controller(GLFWwindow* window, Camera& camera, std::shared_ptr<Vehicle>& playerVehicle, int s);
	~Controller();

	////// toggle the window cursor on!!!
	void toggleCursor();
	bool isCursorShowing;
	
	void setWindowShouldClose(bool close);
	bool isWindowClosed() const;

	//std::queue<int> gameInput();
	void processInput(float deltaSec);
	bool isCameraManual() { return manualCamera; }
	bool isPaused() { return paused; }
	int getIndex() { return index; }
	bool stopLoop() { return breakLoop; }

private:
	GLFWwindow* window;
	Camera& camera;

	std::shared_ptr<Vehicle> playerVehicle;

	bool breakLoop;
	int selection;
	int index;
	bool paused;
	bool manualCamera;
	bool firstMouse;
	float lastX;
	float lastY;
	std::queue<int> currentDrivingControls;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
