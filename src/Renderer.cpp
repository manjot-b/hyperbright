#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <PxPhysicsAPI.h>
#include <filesystem>

#include "Renderer.h"


/*
* Constructs a renderer and initializes GLFW and GLAD. Note that OpenGL functions will
* not be available until GLAD is initialized.
*
* Parameters:
	camera: The camera for the scene. The renderer will not the alter the camera in any way.
*/
Renderer::Renderer(const std::shared_ptr<Camera> camera) : camera(camera)
{
	initWindow();
	shader = std::make_unique<Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();


	perspective = glm::perspective(glm::radians(45.0f), float(width)/height, 0.1f, 100.0f);
	shader->use();
	shader->setUniformMatrix4fv("perspective", perspective);
	shader->setUniformMatrix4fv("view", camera->getViewMatrix());

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

/*
* Renderer a frame.
*
* Parameters:
*	deltaSec: How much time has elapsed in seconds since the last frame was rendered.
*	devUI: An imgui window.
*	models: All the models to renderer this frame.
*/
void Renderer::render(	float deltaSec, DevUI& devUI, 
						std::vector<std::unique_ptr<Model>>& staticModels,
						std::vector<std::unique_ptr<Model>>& physicsModels,
						std::vector<std::unique_ptr<Texture>>& textures)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->use();
	shader->setUniformMatrix4fv("view", camera->getViewMatrix());
	shader->setUniformMatrix4fv("perspective", perspective);

	// set textures for each model by hand
	// boxcar <- awesomeface texture[0]
	textures[0]->bind(GL_TEXTURE0);
	physicsModels[0]->draw(*shader);

	// ground cube <- tree texture[2]
	textures[2]->bind(GL_TEXTURE0);
	staticModels[0]->draw(*shader);

	// background cube <- background texture[1]
	textures[1]->bind(GL_TEXTURE0);
	staticModels[1]->draw(*shader);

	glUseProgram(0);

	devUI.show(deltaSec);

	glfwSwapBuffers(window);
}

void Renderer::updateModelList(std::vector<std::string> _modelList)
{
	modelList = _modelList;
}

