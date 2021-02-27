#pragma once

#define _USE_MATH_DEFINES
#include <glm/gtc/quaternion.hpp>

#include <math.h>
#include <array>
#include <memory>

#include "Model.h"
#include "Pickup.h"
#include "Renderer.h"
#include "Simulate.h"

struct VehicleController {
	int contrId;
	int input[6] = { 0,0,0,0,0,0 };
};

class Vehicle : public Renderer::IRenderable, public IPhysical
{
public:
	Vehicle(const std::string& id, glm::vec4 color, glm::vec3 startPos, glm::vec3 startDir);
	~Vehicle();
	void reset();

	void updatePositionAndDirection();

	const char* getId() const { return id.c_str(); }
	VehicleController getController() { return ctrl; }
	const glm::vec4& getColor() const { return color; }
	glm::vec3 getForward() const { return position + 2.f*direction; }
	const glm::vec3& getPosition() const { return position; }
	glm::quat getOrientation() const;

	void setModelMatrix(const glm::mat4& modelMat);
	void setPosition(const glm::vec3& position);
	void setColor(const glm::vec4 _color) { color = _color; }

	glm::vec2 currentTile;
	float energy;
	bool suckerActive;//IMPLEMENTATION IN COLLISION DETECTION 
	bool syphonActive;//IMPLEMENTATION IN COLLISION DETECTION

	std::shared_ptr<Pickup> pickupEquiped;//set as null for default
	
	// driving movements act as a toggle. call the appropriate movement 
	// function to start moving then call the cooresponding stop function 
	void accelerateForward();
	void accelerateReverse();
	void brake();		// not implemented
	void turnLeft();
	void turnRight();
	void turnHardLeft();// not implemented
	void turnHardRight();// not implemented
	void resetControls();

	void stopForward();
	void stopReverse();
	void stopLeft();
	void stopRight();

	void render(const Shader& shader) const;
private:

	std::string id;
	glm::vec4 color;
	glm::vec3 direction;
	const glm::vec3 startDirection;
	glm::vec3 position;
	VehicleController ctrl;

	std::unique_ptr<Model> body;
	std::unique_ptr<Model> wheelsFront;
	std::unique_ptr<Model> wheelsRear;
};
