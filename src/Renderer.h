#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <memory>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

class Renderer
{
	public:
		class IRenderable
		{
		public:
			virtual void render(const Shader& shader) const = 0;
		};

		Renderer(const Camera& camera);
		~Renderer();

		GLFWwindow* getWindow();

		void render(const std::vector<std::shared_ptr<IRenderable>>& renderables);

	private:
		GLFWwindow* window;

		std::unique_ptr<Shader> shader;

		const unsigned int height = 800;
		const unsigned int width = 800;

		const Camera& camera;
		glm::mat4 perspective;

		void initWindow();
};
