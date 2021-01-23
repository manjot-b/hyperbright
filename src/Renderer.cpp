#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <filesystem>
#include <PxPhysicsAPI.h>

#include "Renderer.h"


/////////////////////////////////////////////////////////////////////////////////////////

using namespace physx;

/////////////////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() :
	modelIndex(0), rotate(0), scale(1),
	firstMouse(true), lastX(width / 2.0f), lastY(height / 2.0f),
	shiftPressed(false), deltaSec(0.0f), lastFrame(0.0f), sceneSelect(0),
	showCursor(false)
{
	initWindow();
	shader = std::make_unique<Shader>("rsc/shaders/vertex.glsl", "rsc/shaders/fragment.glsl");
	shader->link();
	loadModels();	
	
	perspective = glm::perspective(glm::radians(45.0f), float(width)/height, 0.1f, 100.0f);
	shader->use();
	shader->setUniformMatrix4fv("perspective", perspective);
	shader->setUniformMatrix4fv("view", camera.getViewMatrix());

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

	// Let GLFW store pointer to this instance of Renderer.
	glfwSetWindowUserPointer(window, static_cast<void*>(this));

	glfwSetFramebufferSizeCallback(window,
			[](GLFWwindow* window, int newWidth, int newHeight) {

		Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

		glViewport(0, 0, newWidth, newHeight);
		renderer->perspective = glm::perspective(glm::radians(45.0f), float(newWidth)/newHeight, 0.1f, 100.0f);
	});
 
	glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

}

GLFWwindow* Renderer::getWindow() { return window; }

void Renderer::loadModels()
{
	namespace fs = std::filesystem;
	const std::string extension = ".obj";

	unsigned int count = 1;
	for (const auto& entry : fs::directory_iterator("rsc/models"))
	{
		if (entry.is_regular_file() && entry.path().extension() == extension)
		{
			std::cout << "Loading " << entry.path() << "..." << std::flush;
			models.push_back(std::make_unique<Model>(entry.path().string()));
			std::cout << "Done! Index: " << count << "\n";
			count++;
		}
	}
}

void Renderer::run(float _deltaSec, DevUI& devUI)
{
	deltaSec = _deltaSec;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processWindowInput();

	shader->use();
	shader->setUniformMatrix4fv("view", camera.getViewMatrix());
	shader->setUniformMatrix4fv("perspective", perspective);

	texture->bind(GL_TEXTURE0);	// we set the uniform in fragment shader to location 0.

	models[modelIndex]->rotate(rotate);
	models[modelIndex]->scale(scale);
	models[modelIndex]->update();
	models[modelIndex]->draw(*shader);

	glUseProgram(0);

	rotate = glm::vec3(0.0f);
	scale = 1;

	devUI.show(deltaSec);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

/*
 * This method typically runs faster than handling a key callback.
 * So controls like movements should be placed in here.
 */
void Renderer::processWindowInput()
{
	float rotationSpeed = glm::radians(135.0f) * deltaSec;
	float scaleSpeed = 1.0f + 1.0f * deltaSec;
	shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	// Rotations
	if (!shiftPressed)
	{
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
	}

	// Camera Movement
	if (shiftPressed)
	{
		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.processKeyboard(Camera::Movement::FORWARD, deltaSec);
		}

		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.processKeyboard(Camera::Movement::BACKWARD, deltaSec);
		}

		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.processKeyboard(Camera::Movement::RIGHT, deltaSec);
		}

		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.processKeyboard(Camera::Movement::LEFT, deltaSec);
		}

		if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			camera.processKeyboard(Camera::Movement::UP, deltaSec);
		}

		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			camera.processKeyboard(Camera::Movement::DOWN, deltaSec);
		}
	}

	// Scaling
	if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		scale *= scaleSpeed;
	}
	if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		scale /= scaleSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (sceneSelect == 0)
		{
			changeScene(1);
		}
		else if (sceneSelect == 1) 
		{
			changeScene(0);
		}
	}
}

/*
 * Handle keyboard inputs that don't require frequent repeated actions,
 * ex closing window, selecting model etc.
 */ 
void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	
	if (action == GLFW_PRESS)
	{
		switch(key)
		{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				break;

			// Select model
			case GLFW_KEY_1:
			case GLFW_KEY_2:
				renderer->modelIndex = key - GLFW_KEY_1;
				break;

			case GLFW_KEY_SPACE:
				if (mods & GLFW_MOD_CONTROL)
				{
					renderer->showCursor = !renderer->showCursor;
					int cursorMode = renderer->showCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
					glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
				}
				break;
		}
	}
}

void Renderer::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

	if (!renderer->showCursor)
	{
		if (renderer->firstMouse)
		{
			renderer->lastX = xpos;
			renderer->lastY = ypos;
			renderer->firstMouse = false;
		}

		float xoffset = xpos - renderer->lastX;
		float yoffset = renderer->lastY - ypos; // reversed since y-coordinates go from bottom to top

		renderer->lastX = xpos;
		renderer->lastY = ypos;

		renderer->camera.processMouseMovement(xoffset, yoffset);
	}
}

bool Renderer::isWindowClosed() const
{
	return glfwWindowShouldClose(window);
}
