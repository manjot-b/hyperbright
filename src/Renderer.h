#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include <memory>

#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

class Renderer
{
	public:
		Renderer();
		~Renderer();
		void run();

	private:
		GLFWwindow* window;
		std::unique_ptr<Shader> shader;
		std::unique_ptr<Texture> texture;
		std::vector<std::unique_ptr<Model>> models;
		unsigned int modelIndex;
		
		const unsigned int height = 800;
		const unsigned int width = 800;

		glm::vec3 rotate;
		float scale;
		Camera camera;
		glm::mat4 perspective;

		bool firstMouse;
		float lastX;
		float lastY;
		bool shiftPressed;

		float deltaTime;
		float lastFrame;

		void initWindow();
		void loadModels();
		void processWindowInput();
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
};
