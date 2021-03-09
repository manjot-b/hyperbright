#include "engine/Engine.h"

int main(int argc, char *argv[])
{
	{
		Engine engine;
		engine.run();
	}
	// Need to terminate GLFW context after all OpenGL objects are deleted.
	// Otherwise, a seg fault will occur.
	glfwTerminate();
}
