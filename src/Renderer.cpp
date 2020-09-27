#include <glad/glad.h>
#include <iostream>
#include "Renderer.h"

Renderer::Renderer(std::vector<std::string> objPaths)
{
	initWindow();
	Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
	shader.link();
	for(const auto& path : objPaths)
	{
		models.emplace_back(path, false, true, shader);
	}	
}

Renderer::~Renderer() {}

void Renderer::initWindow()
{
	// Setup glfw context
	constexpr unsigned int height = 600;
	constexpr unsigned int width = 800;
	constexpr float aspectRatio = float(width) / height;

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

		for(const auto& model : models)
		{
			model.draw();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();	
}

void Renderer::processWindowInput()
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

