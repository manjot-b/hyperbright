#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include "Model.h"

class Renderer
{
	public:
		Renderer(std::vector<std::string> objPaths);
		~Renderer();
		void run();

	private:
		GLFWwindow* window;
		std::vector<Model> models;
		void initWindow();
		void processWindowInput();
};
