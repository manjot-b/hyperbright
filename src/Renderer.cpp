#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"

Renderer::Renderer(std::vector<std::string> objPaths) :
	rotate(0), scale(1)
{
	initWindow();
	Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
	shader.link();
	for(const auto& path : objPaths)
	{
		models.emplace_back(path, shader);
	}	
	
	perspective = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	view = glm::lookAt(
				glm::vec3(0, 2, 3),	// camera position
				glm::vec3(0, 0, 0),	// camera direction
				glm::vec3(0, 1, 0)	// up direction
			);
	shader.use();
	shader.setUniformMatrix4fv("perspective", perspective);
	shader.setUniformMatrix4fv("view", view);
	glUseProgram(0);	// unbind shader
}

Renderer::~Renderer() {}

void Renderer::initWindow()
{
	// Setup glfw context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		float aspectRatio = width / height;
		float viewPortHeight = (1/aspectRatio) * width;
		float viewPortWidth = width;
		float xPos = 0;
		float yPos = 0;

		if(viewPortHeight > height)
		{
			viewPortHeight = height;
			viewPortWidth = aspectRatio * height;
			xPos = (width - viewPortWidth) / 2.0f;	
		}
		else
		{
			yPos = (height - viewPortHeight) / 2.0f;
		}

		glViewport(xPos, yPos, viewPortWidth, viewPortHeight);
	});
	glEnable(GL_DEPTH_TEST);

}

void Renderer::run()
{

	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);

		processWindowInput();

		for(auto& model : models)
		{
			model.rotate(rotate);
			model.scale(scale);
			model.update();
			model.draw();
		}
		rotate = glm::vec3(0.0f);
		scale = 1;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Renderer::processWindowInput()
{
	float rotationSpeed = glm::radians(3.0f);
	float scaleSpeed = 1.01;

	// Close window
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// Rotations
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		rotate.x -= rotationSpeed;
	}

	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		rotate.x += rotationSpeed;
	}

	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		rotate.y += rotationSpeed;
	}

	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		rotate.y -= rotationSpeed;
	}

	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		rotate.z -= rotationSpeed;
	}

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		rotate.z += rotationSpeed;
	}

	if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		scale *= scaleSpeed;
	}
	if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		scale /= scaleSpeed;
	}
}

