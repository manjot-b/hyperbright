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
		void run(float deltaSec, DevUI& devUI, std::vector<std::unique_ptr<Model>>& models);

		void setWindowShouldClose(bool close);
		bool isWindowClosed() const;
		void toggleCursor();
		bool isCursorShowing() const;

	private:
		GLFWwindow* window;

		std::unique_ptr<Shader> shader;
		std::unique_ptr<Texture> texture;
		
		const unsigned int height = 800;
		const unsigned int width = 800;

		const std::shared_ptr<Camera> camera;
		glm::mat4 perspective;

		bool showCursor;

		void initWindow();
};
