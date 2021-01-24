#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <PxPhysicsAPI.h>

#include "Renderer.h"


/////////////////////////////////////////////////////////////////////////////////////////

using namespace physx;

/////////////////////////////////////////////////////////////////////////////////////////

Renderer::Renderer(const std::shared_ptr<Camera> camera) : camera(camera), showCursor(false)
{
	initWindow();
	shader = std::make_unique<Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();
	
	
	perspective = glm::perspective(glm::radians(45.0f), float(width)/height, 0.1f, 100.0f);
	shader->use();
	shader->setUniformMatrix4fv("perspective", perspective);
	shader->setUniformMatrix4fv("view", camera->getViewMatrix());

	texture = std::make_unique<Texture>("rsc/images/tree.jpeg");
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
		renderer->perspective = glm::perspective(glm::radians(45.0f), float(newWidth)/newHeight, 0.1f, 100.0f);
	});
 
	glEnable(GL_DEPTH_TEST);

}

GLFWwindow* Renderer::getWindow() { return window; }

void Renderer::run(float deltaSec, DevUI& devUI, std::vector<std::unique_ptr<Model>>& models)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->use();
	shader->setUniformMatrix4fv("view", camera->getViewMatrix());
	shader->setUniformMatrix4fv("perspective", perspective);

	texture->bind(GL_TEXTURE0);	// we set the uniform in fragment shader to location 0.

	for (auto& model : models)
	{
		model->draw(*shader);
	}

	glUseProgram(0);

	devUI.show(deltaSec);

	glfwSwapBuffers(window);
}

void Renderer::setWindowShouldClose(bool close)
{
	glfwSetWindowShouldClose(window, close);
}

bool Renderer::isWindowClosed() const
{
	return glfwWindowShouldClose(window);
}

void Renderer::toggleCursor()
{
	showCursor = !showCursor;
	int cursorMode = showCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
	glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
}

bool Renderer::isCursorShowing() const
{
	return showCursor;
}