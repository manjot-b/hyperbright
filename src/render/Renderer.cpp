#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


namespace hyperbright {
namespace render {
/*
* Constructs a renderer and initializes GLFW and GLAD. Note that OpenGL functions will
* not be available until GLAD is initialized.
*/
Renderer::Renderer()
{
	initWindow();
	shader = std::make_unique<openGLHelper::Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();


	perspective = glm::perspective(glm::radians(45.0f), float(width)/height, 0.1f, 500.0f);
	shader->use();
	shader->setUniformMatrix4fv("perspective", perspective);

	std::vector<Light> lights = {
		{false, glm::vec3(-1.f, -1.f, 1.f), glm::vec3(.4f, .4f, .5f)},
		{true, glm::vec3(-40.f, 10.f, -30.f), glm::vec3(.7f, .7f, .1f), 1.f, .014f, 0.0007f},
		{true, glm::vec3(0.f, 10.f, 0.f), glm::vec3(.7f, .7f, .1f), 1.f, .014f, 0.0007f}
	};
	shader->setUniform1i("lightCount", lights.size());
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		std::string isPointUniform = "lights[" + std::to_string(i) + "]" + ".isPoint";
		std::string positionUniform = "lights[" + std::to_string(i) + "]" + ".position";
		std::string colorUniform = "lights[" + std::to_string(i) + "]" + ".color";
		std::string constantUniform = "lights[" + std::to_string(i) + "]" + ".constant";
		std::string linearUniform = "lights[" + std::to_string(i) + "]" + ".linear";
		std::string quadraticUniform = "lights[" + std::to_string(i) + "]" + ".quadratic";

		shader->setUniform1i(isPointUniform.c_str(), lights[i].isPoint);
		shader->setUniform3fv(positionUniform.c_str(), lights[i].position);
		shader->setUniform3fv(colorUniform.c_str(), lights[i].color);
		shader->setUniform1f(constantUniform.c_str(), lights[i].constant);
		shader->setUniform1f(linearUniform.c_str(), lights[i].linear);
		shader->setUniform1f(quadraticUniform.c_str(), lights[i].quadratic);
	}

	shader->setUniform1i("tex", 0);	// sets location of texture to 0.

	glUseProgram(0);	// unbind shader
}

Renderer::~Renderer() {}

Renderer& Renderer::getInstance()
{
	static Renderer renderer;	// instantiated only once.
	return renderer;
}

void Renderer::initWindow()
{
	// Setup glfw context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	window = glfwCreateWindow(width, height, "OpenGL Example", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(window,
			[](GLFWwindow* window, int newWidth, int newHeight) {
		glViewport(0, 0, newWidth, newHeight);
		Renderer::getInstance().perspective = glm::perspective(glm::radians(45.0f), float(newWidth)/newHeight, 0.1f, 500.0f);
	});

	glEnable(GL_DEPTH_TEST);

}

GLFWwindow* Renderer::getWindow() { return window; }

/*
* Renderer a frame.
*
* Parameters:
*	renderables: A vector of IRenderables that will be drawn.
*	devUI: An imgui window.
*	menu: The menu object.
*/

void Renderer::render(const std::vector<std::shared_ptr<IRenderable>>& renderables, ui::DevUI& devUI, ui::Menu& menu, const Camera& camera)
{
	glClearColor(0.05f, 0.05f, 0.23f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->use();
	shader->setUniformMatrix4fv("view", camera.getViewMatrix());
	shader->setUniformMatrix4fv("perspective", perspective);
	shader->setUniform3fv("pointOfView", camera.getPosition());

	for (const auto& renderable : renderables)
	{
		renderable->render(*shader);
	}

	glUseProgram(0);

	menu.render();
	devUI.render();

	glfwSwapBuffers(window);
}
}   // namespace render
}   // namespace hyperbright