#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"
#include "VertexArray.h"

void processInput(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

int main()
{
	// Setup glfw context
	constexpr unsigned int height = 600;
	constexpr unsigned int width = 800;
	constexpr float aspectRatio = float(width) / height;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Example", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
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

	// Triangle data
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};	

	//Create the VAO
	const int vertexComponents = 3;
	VertexArray vertexArray = VertexArray(&vertexComponents,
			1,
			vertices,
			sizeof(vertices)/sizeof(float));

	Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
	shader.link();

	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window);

		shader.use();
		vertexArray.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();	
}
