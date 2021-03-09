#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class DevUI
{
public:
	DevUI(GLFWwindow* window);
	~DevUI();
	void update(float deltaSec, float roundTimer);
	void render();
	int getSliderFPS();
private:
	bool showDemo;
	float _deltaSec;
	float _roundTimer;
};
