#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DevUI.h"

class Renderer
{
	public:
		Renderer(const std::shared_ptr<Camera> camera);
		~Renderer();

		GLFWwindow* getWindow();
		void render(float deltaSec, DevUI& devUI, std::vector<std::unique_ptr<Model>>& models);

	private:
		GLFWwindow* window;

		std::unique_ptr<Shader> shader;
		std::unique_ptr<Texture> texture;

		const unsigned int height = 800;
		const unsigned int width = 800;

		const std::shared_ptr<Camera> camera;
		glm::mat4 perspective;

		void initWindow();
};
