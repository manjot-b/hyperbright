#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <queue>

#include "audio/AudioPlayer.h"
#include "render/Camera.h"
#include "ui/Menu.h"
#include "entity/Vehicle.h"

namespace hyperbright {
namespace engine {
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

	Controller(GLFWwindow* window,
		render::Camera& camera,
		ui::MainMenu& mainmenu,
		ui::PauseMenu& pausemenu,
		ui::EndMenu& endmenu,
		audio::AudioPlayer& audioPlayer);
	~Controller();

	////// toggle the window cursor on!!!
	void toggleCursor();
	bool isCursorShowing;

	void setWindowShouldClose(bool close);
	bool isWindowClosed() const;

	//std::queue<int> gameInput();
	void processInput(float deltaSec);
	bool isCameraManual() { return manualCamera; }

	void setPlayerVehicle(std::shared_ptr<entity::Vehicle>& vehicle);
	bool altDriveControls = false;

private:
	GLFWwindow* window;
	render::Camera& camera;
	ui::MainMenu& mainMenu;
	ui::PauseMenu& pauseMenu;
	ui::EndMenu& endMenu;
	audio::AudioPlayer& audioPlayer;

	std::shared_ptr<entity::Vehicle> playerVehicle;

	bool manualCamera;
	bool firstMouse;
	float lastX;
	float lastY;
	std::queue<int> currentDrivingControls;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mainMenuKeyCallback(int key, int scancode, int action, int mods);
	void pauseMenuKeyCallback(int key, int scancode, int action, int mods);
	void gameKeyCallback(int key, int scancode, int action, int mods);
	void endMenuKeyCallback(int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
}	// namespace engine
}	// namespace hyperbright