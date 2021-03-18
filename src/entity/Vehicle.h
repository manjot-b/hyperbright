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
};

class Vehicle : public render::Renderer::IRenderable, public physics::IPhysical
{
public:
	Vehicle(const std::string& id,
		engine::teamStats::Teams team,
		const std::shared_ptr<openGLHelper::Shader>& shader,
		glm::vec3 startPos,
		glm::vec3 startDir);
	~Vehicle();
	void reset();

	void updatePositionAndDirection();

	const char* getId() const { return id.c_str(); }
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

	void setModelMatrix(const glm::mat4& modelMat);
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
	void turnHardLeft();// not implemented
	void turnHardRight();// not implemented
	void resetControls();

	void stopForward();
	void stopReverse();
	void stopLeft();
	void stopRight();
	int index;
	void render() const;
private:

	std::string id;
	engine::teamStats::Teams team;
	glm::vec4 color;
	glm::vec3 direction;
	const glm::vec3 startDirection;
	glm::vec3 position;
	VehicleController ctrl;

	std::unique_ptr<model::Model> body;
	std::unique_ptr<model::Model> wheel;
	unsigned int bodyIdx;
	unsigned int brakeLightsIdx;
};
}	// namespace entity
}	// namespace hyperbright