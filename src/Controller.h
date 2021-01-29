#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Vehicle.h"
#include "Ai.h"
#include "Camera.h"

class Controller
{
public:
	Controller(GLFWwindow* window, std::shared_ptr<Camera> camera);
	~Controller();

	// Temporary model selection 
	int modelIndex;
	void nextModel();
	int numModels;

	////// toggle the window cursor on!!!
	void toggleCursor();
	bool isCursorShowing;
	
	void setWindowShouldClose(bool close);
	bool isWindowClosed() const;

	void gameInput(Vehicle vehicle);
	void processInput(float deltaSec);
	bool isCameraManual() { return manualCamera; }

private:
	GLFWwindow* window;
	std::shared_ptr<Camera> camera;

	bool manualCamera;
	bool firstMouse;
	float lastX;
	float lastY;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
