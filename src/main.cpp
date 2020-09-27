#include <glad/glad.h>
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

	Renderer renderer(objPaths);
	renderer.run();
}
