#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class DevUI
{
public:
	DevUI(GLFWwindow* window);
	~DevUI();
	void show(float deltaSec);
	int getSliderFPS();
private:
	bool showDemo;
};
