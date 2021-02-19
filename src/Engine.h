#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>
#include <vector>


#include "AudioPlayer.h"

#include "Arena.h"

#include "Model.h"
#include "Renderer.h"
#include "Texture.h"
#include "Vehicle.h"

class Engine
{
public:
	Engine();
	~Engine();
	void run();
private:
	Camera camera;
	Renderer renderer;
	std::shared_ptr<Arena> arena;

	float deltaSec;
	//Ai aiPlayers[4];
	//Vehicle vehicles[4];
	//Pickup pickups[10];

	std::vector<std::shared_ptr<Vehicle>> vehicles;
	std::vector<std::shared_ptr<Model>> staticModels;
	std::vector<std::shared_ptr<Model>> physicsModels;
	std::vector<std::shared_ptr<Renderer::IRenderable>> renderables;

	// These should eventually be their specific classes rather than Model.
	// e.g. the plane should be Arena, car should be Vehicle.
	std::shared_ptr<Model> vehicle;
	std::shared_ptr<Model> ai1;
	std::shared_ptr<Model> powerup;
	std::shared_ptr<Model> triggerVolume;

	// Rename/remove as required.
	std::shared_ptr<Texture> face;
	std::shared_ptr<Texture> tree;
	std::shared_ptr<Texture> background;

	std::shared_ptr<AudioPlayer> audioPlayer;
	void setupAudioPlayer();

	glm::vec3 rotate;
	float scale;

	float lastFrame;

	void runMenu();
	int menuInput();
	void runGame();
  
	std::shared_ptr<Model> loadModel(std::string ref,
		bool inPhysx,
		const char* name,
		const std::shared_ptr<Texture>& texture,
		const glm::vec4& color = glm::vec4(0.3, 0.3, 0.3, 0),
		bool copyModel = false);
	void loadTextures();
	void initEntities();
};
