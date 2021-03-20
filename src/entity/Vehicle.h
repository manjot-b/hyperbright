#pragma once

#define _USE_MATH_DEFINES
#include <glm/gtc/quaternion.hpp>

#include <math.h>
#include <array>
#include <memory>

#include "model/Model.h"
#include "entity/Pickup.h"
#include "render/Renderer.h"
#include "physics/Simulate.h"
#include "engine/TeamStats.h"

namespace hyperbright {
namespace entity {

struct VehicleController {
	int contrId;
	int input[6] = { 0,0,0,0,0,0 };
	// input controls { accelerate, reverse, turn right, turn left, hard turn, brake }
};

class Vehicle : public render::Renderer::IRenderable, public physics::IPhysical
{
public:
	Vehicle(
		engine::teamStats::Teams team,
		const std::shared_ptr<openGLHelper::Shader>& shader,
		glm::vec3 startPos,
		glm::vec3 startDir);
	~Vehicle();
	void reset();

	void updatePositionAndDirection();

	const char* getId() const { return engine::teamStats::names[team].c_str(); }
	VehicleController getController() { return ctrl; }
	const glm::vec4& getColor() const { return color; }
	glm::vec3 getForward() const { return position + direction; }
	glm::vec3 getDirection() const { return direction; }
	const glm::vec3& getPosition() const { return position; }
	glm::quat getOrientation() const;

	engine::teamStats::Teams getTeam() const { return team; }

	void setBodyMaterial(const model::Material& material);
	const model::Material& getBodyMaterial() const;

	void reduceEnergy();
	void restoreEnergy();
	bool enoughEnergy();

	void activatePickup();

	void setModelMatrix(const glm::mat4& modelMat);
	void setWheelsModelMatrix(const glm::mat4& frontLeft, const glm::mat4& frontRight, const glm::mat4& rearRight, const glm::mat4& rearLeft);
	void setPosition(const glm::vec3& position);
	void setColor(const glm::vec4 _color) { color = _color; }

	glm::vec2 currentTile;
	float energy = 1.f;
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
	void hardTurn();// not implemented
	void resetControls();

	void stopForward();
	void stopReverse();
	void stopBrake();
	void stopLeft();
	void stopRight();
	void stopHardTurn();

	float updateSpeedometer(float deltaTime);

	int index;
	void render() const;
private:

	engine::teamStats::Teams team;
	glm::vec4 color;
	glm::vec3 direction;
	const glm::vec3 startDirection;
	glm::vec3 position;
	glm::vec3 lastPosition;
	float speedometer;
	VehicleController ctrl;

	std::unique_ptr<model::Model> body;
	// Follow the order from physx. Front left, front right, rear left, rear right.
	std::array<std::unique_ptr<model::Model>, 4> wheels;
	unsigned int bodyIdx;
	unsigned int brakeLightsIdx;
};
}	// namespace entity
}	// namespace hyperbright