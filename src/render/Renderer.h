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

#include "ui/HUD.h"
#include "ui/LoadingScreen.h"
#include "opengl-helper/FrameBuffer.h"
#include "opengl-helper/Quad.h"
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

		void render(const std::vector<std::shared_ptr<IRenderable>>& renderables, ui::DevUI& devUI, ui::Menu& menu, const Camera& camera, ui::HUD* hud);
		void render(const ui::LoadingScreen& loadingScreen);

	private:
		GLFWwindow* window;

		// 16:9 aspect ratio
		unsigned int height = 675;
		unsigned int width = 1200;

		glm::mat4 perspective;

		// matrices for the direction light shadow view.
		glm::mat4 lightProjection;
		glm::mat4 lightView;
		std::shared_ptr<openGLHelper::Shader> shadowShader;
		std::shared_ptr<openGLHelper::Texture> shadowMap;
		std::unique_ptr<openGLHelper::FrameBuffer> shadowBuffer;
		Light directionalLight;

		std::shared_ptr<openGLHelper::Shader> quadShader;
		std::unique_ptr<openGLHelper::Quad> texturedQuad;

		void initWindow();
};
}   // namespace render
}   // namespace hyperbright