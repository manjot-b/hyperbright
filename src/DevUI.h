#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Renderer.h"

class DevUI
{
public:
	DevUI(GLFWwindow* window);
	~DevUI();
	void update(float deltaSec);
	void render(const Shader& shader);
	int getSliderFPS();
private:
	bool showDemo;
	float _deltaSec;
};
