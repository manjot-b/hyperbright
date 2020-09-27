#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>

#include "Renderer.h"

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <objpath1> <objpath2>..." << std::endl;
		return -1;
	}

	std::vector<std::string> objPaths;
	for (int i = 1; i < argc; i++)
	{
		objPaths.push_back(argv[i]);
	}

	{
		Renderer renderer(objPaths);
		renderer.run();
	}
	// Need to terminate GLFW context after all OpenGL objects are deleted.
	// Otherwise, a seg fault will occur.
	glfwTerminate();
}
