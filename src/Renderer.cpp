#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <filesystem>

/*
* Constructs a renderer and initializes GLFW and GLAD. Note that OpenGL functions will
* not be available until GLAD is initialized.
*
* Parameters:
	camera: The camera for the scene. The renderer will not the alter the camera in any way.
*/
Renderer::Renderer(const Camera& camera) : camera(camera)
{
	initWindow();
	shader = std::make_unique<Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();


	perspective = glm::perspective(glm::radians(45.0f), float(width)/height, 0.1f, 200.0f);
	shader->use();
	shader->setUniformMatrix4fv("perspective", perspective);
	shader->setUniformMatrix4fv("view", camera.getViewMatrix());

	shader->setUniform3fv("light", glm::vec3(10.f, 10.f, 0.f));
	shader->setUniform3fv("pointOfView", camera.getPosition());
	shader->setUniform1f("d", 1.f);

	shader->setUniform1i("tex", 0);	// sets location of texture to 0.

	glUseProgram(0);	// unbind shader
}

Renderer::~Renderer() {}

void Renderer::initWindow()
{
	// Setup glfw context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

		Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

		glViewport(0, 0, newWidth, newHeight);
		renderer->perspective = glm::perspective(glm::radians(45.0f), float(newWidth)/newHeight, 0.1f, 200.0f);
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
*/

void Renderer::render(const std::vector<std::shared_ptr<IRenderable>>& renderables, DevUI& devUI)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->use();
	shader->setUniformMatrix4fv("view", camera.getViewMatrix());
	shader->setUniformMatrix4fv("perspective", perspective);


	for (const auto& renderable : renderables)
	{
		renderable->render(*shader);
	}

	glUseProgram(0);

	devUI.render();

	glfwSwapBuffers(window);
}
