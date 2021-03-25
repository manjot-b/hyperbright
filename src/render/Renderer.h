#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <memory>

#include "Camera.h"
#include "IRenderable.h"
#include "Light.h"
#include "ui/DevUI.h"
#include "ui/Menu.h"
#include "opengl-helper/Shader.h"
#include "opengl-helper/Texture.h"

namespace hyperbright {
namespace render {
class Renderer
{
	public:
		Renderer();
		~Renderer();
		static Renderer& getInstance();

		// The renderer is a singleton.
		Renderer(const Renderer&) = delete;
		void operator=(const Renderer&) = delete;

		GLFWwindow* getWindow();
		void getWindowSize(unsigned int& width, unsigned int& height);
		void setWindowSize(unsigned int width, unsigned int height);
		void initShaderUniforms(const std::shared_ptr<openGLHelper::Shader> shader);

		void render(const std::vector<std::shared_ptr<IRenderable>>& renderables, ui::DevUI& devUI, ui::Menu& menu, const Camera& camera);

	private:
		GLFWwindow* window;

		// 16:9 aspect ratio
		unsigned int height = 675;
		unsigned int width = 1200;

		glm::mat4 perspective;

		void initWindow();
};
}   // namespace render
}   // namespace hyperbright