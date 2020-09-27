#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

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
		
		static const unsigned int height = 600;
		static const unsigned int width = 800;
		const float aspectRatio = float(width) / height;

		glm::vec3 rotate;
		float scale;
		glm::mat4 view;
		glm::mat4 perspective;

		void initWindow();
		void processWindowInput();
};
