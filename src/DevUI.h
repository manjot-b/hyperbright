#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

class DevUI
{
public:
	DevUI(GLFWwindow* window);
	~DevUI();
	void show(float deltaSec);
private:
	bool showDemo;
};
