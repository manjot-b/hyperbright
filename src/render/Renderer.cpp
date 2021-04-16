#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <iostream>


namespace hyperbright {
namespace render {

/*
* Constructs a renderer and initializes GLFW and GLAD. Note that OpenGL functions will
* not be available until GLAD is initialized.
*/
Renderer::Renderer()
{
	initWindow();

	shadowShader = std::make_shared<openGLHelper::Shader>("rsc/shaders/shadow_vertex.glsl", "rsc/shaders/shadow_fragment.glsl");
	shadowShader->link();
	shadowMap = std::make_shared<openGLHelper::Texture>(4096, 4096, true);
	shadowBuffer = std::make_unique<openGLHelper::FrameBuffer>(shadowMap, true);

	perspective = glm::perspective(glm::radians(45.0f), float(width)/height, 0.1f, 1000.0f);

	// quad render test
	quadShader = std::make_shared<openGLHelper::Shader>("rsc/shaders/quad_vertex.glsl", "rsc/shaders/quad_fragment.glsl");
	quadShader->link();
	texturedQuad = std::make_unique<openGLHelper::Quad>(quadShader, shadowMap);
}

Renderer::~Renderer() {}

Renderer& Renderer::getInstance()
{
	static Renderer renderer;	// instantiated only once.
	return renderer;
}

void Renderer::initWindow()
{
	// Setup glfw context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	width = mode->width;
	height = mode->height;


	int n;
	GLFWimage icon[1];
	unsigned char* data = stbi_load("rsc/images/icon.png", &icon[0].width, &icon[0].height, &n, 0);
	icon[0].pixels = data;

	int n;
	GLFWimage icon[1];
	unsigned char* data = stbi_load("rsc/images/test.png", &icon[0].width, &icon[0].height, &n, 0);
	icon[0].pixels = data;

	window = glfwCreateWindow(width, height, "HyperBright", primaryMonitor, nullptr);

	glfwSetWindowIcon(window, 1, icon);
	stbi_image_free(icon[0].pixels);

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
		if (newWidth && newHeight)
		{
			glViewport(0, 0, newWidth, newHeight);
			Renderer::getInstance().perspective = glm::perspective(glm::radians(45.0f), float(newWidth) / newHeight, 0.1f, 1000.0f);
			Renderer::getInstance().setWindowSize(newWidth, newHeight);
		}
	});

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

}

GLFWwindow* Renderer::getWindow() { return window; }

void Renderer::getWindowSize(unsigned int& width, unsigned int& height)
{
	width = Renderer::getInstance().width;
	height = Renderer::getInstance().height;
}

void Renderer::setWindowSize(unsigned int width, unsigned int height)
{
	Renderer::getInstance().width = width;
	Renderer::getInstance().height = height;
}

/*
 Initializes the shader uniforms that only need to be set once, not every frame.
*/
void Renderer::initShaderUniforms(const std::shared_ptr<openGLHelper::Shader> shader)
{
	shader->use();
	shader->setUniformMatrix4fv("perspective", perspective);

	directionalLight = { false, glm::vec3(-.5f, -.5f, 1.f), glm::vec3(.4f, .4f, .5f) };

	// TO-DO: Use actual arena size to contruct light's orthgraphic and view matrices.
	float orthoSize = 60.f;
	lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, .1f, 100.f);
	lightView = glm::lookAt(
		-directionalLight.position * 40.f,	// tmp hardcoded position
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);

	std::vector<Light> lights = {
		directionalLight,
		{true, glm::vec3(-40.f, 10.f, -30.f), glm::vec3(.7f, .7f, .1f), 1.f, .014f, 0.0007f},
		{true, glm::vec3(0.f, 10.f, 0.f), glm::vec3(.7f, .7f, .1f), 1.f, .014f, 0.0007f}
	};
	shader->setUniform1i("lightCount", lights.size());
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		std::string isPointUniform = "lights[" + std::to_string(i) + "]" + ".isPoint";
		std::string positionUniform = "lights[" + std::to_string(i) + "]" + ".position";
		std::string colorUniform = "lights[" + std::to_string(i) + "]" + ".color";
		std::string constantUniform = "lights[" + std::to_string(i) + "]" + ".constant";
		std::string linearUniform = "lights[" + std::to_string(i) + "]" + ".linear";
		std::string quadraticUniform = "lights[" + std::to_string(i) + "]" + ".quadratic";

		shader->setUniform1i(isPointUniform.c_str(), lights[i].isPoint);
		shader->setUniform3fv(positionUniform.c_str(), lights[i].position);
		shader->setUniform3fv(colorUniform.c_str(), lights[i].color);
		shader->setUniform1f(constantUniform.c_str(), lights[i].constant);
		shader->setUniform1f(linearUniform.c_str(), lights[i].linear);
		shader->setUniform1f(quadraticUniform.c_str(), lights[i].quadratic);
	}

	shader->setUniform1i("tex", 0);	// sets location of texture to 0.
	shader->setUniform1i("shadowMap", 1);

	glUseProgram(0);	// unbind shader
}

/*
* Renderer a frame.
*
* Parameters:
*	renderables: A vector of IRenderables that will be drawn.
*	devUI: An imgui window.
*	menu: The menu object.
*/

void Renderer::render(const std::vector<std::shared_ptr<IRenderable>>& renderables, ui::DevUI& devUI, ui::Menu& menu, const Camera& camera, ui::HUD* hud)
{
	// Render to the shadow map first.
	lightView = glm::lookAt(
		camera.getPosition() + -directionalLight.position * 40.f,	// move with the camera position
		camera.getPosition(),
		glm::vec3(0.f, 1.f, 0.f)
	);
	shadowShader->use();
	shadowShader->setUniformMatrix4fv("projection", lightProjection);
	shadowShader->setUniformMatrix4fv("view", lightView);

	glViewport(0, 0, shadowMap->getWidth(), shadowMap->getHeight());
	shadowBuffer->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	for (const auto& renderable : renderables)
	{
		renderable->renderShadow(shadowShader);
	}

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

	shadowMap->bind(GL_TEXTURE1);

	// Render regular scene
	glClearColor(0.05f, 0.05f, 0.23f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& renderable : renderables)
	{
		renderable->getShader()->use();
		renderable->sendSharedShaderUniforms(perspective, camera.getViewMatrix(), camera.getPosition(), lightProjection * lightView);
		renderable->render();
	}

	/*
	quadShader->use();
	texturedQuad->normalizeToViewport(width, height);
	texturedQuad->render();
	*/

	glUseProgram(0);

	// Render UI elements
	menu.render();
	if (hud) {
		
		hud->preRenderMiniMap();

		// Renders the scene to the minimaps framebuffer.
		for (const auto& renderable : renderables)
		{
			renderable->getShader()->use();
			renderable->sendSharedShaderUniforms(hud->getMiniMapOrtho(), hud->getMiniMapCameraView(), hud->getMiniMapCameraPos(), lightProjection * lightView);
			renderable->renderMiniMap();
		}

		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		
		
		hud->drawHUD();
	}
	devUI.render();

	glfwSwapBuffers(window);
}
}   // namespace render
}   // namespace hyperbright