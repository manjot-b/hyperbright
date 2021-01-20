#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>

//#include "Renderer.h"
#include "Engine.h"

int main(int argc, char *argv[])
{
	{
		Engine engine;
		engine.run();
		//Renderer renderer;
		//renderer.run();
	}
	// Need to terminate GLFW context after all OpenGL objects are deleted.
	// Otherwise, a seg fault will occur.
	glfwTerminate();
}
