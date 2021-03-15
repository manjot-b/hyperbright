#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "model/MeshUtils.h"

namespace hyperbright {
namespace ui {
class DevUI
{
public:
	struct Settings {
		int fps;
		model::Material vehicleBodyMaterial;
	} settings;

	DevUI(GLFWwindow* window);
	~DevUI();
	void update(float deltaSec, float roundTimer);
	void render();
private:
	bool showDemo;
	float _deltaSec;
	float _roundTimer;
};
}	// namespace ui
}	// namespace hyperbright