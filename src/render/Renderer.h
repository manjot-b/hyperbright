#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <memory>

#include "Camera.h"
#include "Light.h"
#include "ui/DevUI.h"
#include "ui/Menu.h"
#include "opengl-helper/Shader.h"
#include "opengl-helper/Texture.h"

class Renderer
{
	public:
		class IRenderable
		{
		public:
			virtual void render(const Shader& shader) const = 0;
		};

		Renderer();
		~Renderer();
		static Renderer& getInstance();

		// The renderer is a singleton.
		Renderer(const Renderer&) = delete;
		void operator=(const Renderer&) = delete;

		GLFWwindow* getWindow();

		void render(const std::vector<std::shared_ptr<IRenderable>>& renderables, DevUI& devUI, Menu& menu, const Camera& camera);

	private:
		GLFWwindow* window;

		std::unique_ptr<Shader> shader;

		// 16:9 aspect ratio
		const unsigned int height = 675;
		const unsigned int width = 1200;

		glm::mat4 perspective;

		void initWindow();
};
