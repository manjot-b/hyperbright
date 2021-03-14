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

namespace hyperbright {
namespace render {
class Renderer
{
	public:
		class IRenderable
		{
		public:
			IRenderable();
			IRenderable(const std::shared_ptr<openGLHelper::Shader>& shader);
			virtual void render() const = 0;
			const std::shared_ptr<openGLHelper::Shader>& getShader() const;
			void setShader(const std::shared_ptr<openGLHelper::Shader>& shader);
			virtual void sendSharedShaderUniforms(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& cameraPos) const;
		protected:
			std::shared_ptr<openGLHelper::Shader> _shader;
		};

		Renderer();
		~Renderer();
		static Renderer& getInstance();

		// The renderer is a singleton.
		Renderer(const Renderer&) = delete;
		void operator=(const Renderer&) = delete;

		GLFWwindow* getWindow();
		void initShaderUniforms(const std::shared_ptr<openGLHelper::Shader> shader);

		void render(const std::vector<std::shared_ptr<IRenderable>>& renderables, ui::DevUI& devUI, ui::Menu& menu, const Camera& camera);

	private:
		GLFWwindow* window;

		// 16:9 aspect ratio
		const unsigned int height = 675;
		const unsigned int width = 1200;

		glm::mat4 perspective;

		void initWindow();
};
}   // namespace render
}   // namespace hyperbright