#pragma once

#define _USE_MATH_DEFINES
#include <glm/gtc/quaternion.hpp>

#include <math.h>
#include <array>
#include <memory>

#include "model/Model.h"
#include "entity/Pickup.h"
#include "render/IRenderable.h"
#include "physics/Interface.h"
#include "engine/TeamStats.h"

namespace hyperbright {
namespace entity {

struct VehicleController {
	int contrId;
	/*input[6] = { forward, reverse, right, left, handbrake, brake }*/
	int input[6] = { 0,0,0,0,0,0 };
	bool flipImpulse = false;
	std::pair<int, bool> boost = std::make_pair(0, false);
	bool trapped = false;
	// straighten: 0 => do nothing, 1 => left correction, 2 => right correction
	int straighten = 0;
};

class Vehicle : public render::IRenderable, public physics::IPhysical
{
public:
	Vehicle(
		engine::teamStats::Teams team,
		const std::shared_ptr<openGLHelper::Shader>& shader,
		glm::vec3 startPos,
		glm::vec3 startDir);
	~Vehicle();
	void reset();

	VehicleController& getController()		{ return ctrl; }
	const glm::vec4& getColor() const		{ return color; }
	glm::vec3 getForward() const			{ return body->getPosition() + direction; }
	glm::vec3 getDirection() const			{ return direction; }
	glm::vec3 getUp() const					{ return up; }
	const glm::vec3& getPosition() const	{ return body->getPosition(); }
	float readSpeedometer()					{ return speedometer; }
	engine::teamStats::Teams getTeam() const{ return team; }
	void setTeam(engine::teamStats::Teams t){ team = t; }


	void updateOrientation();
	glm::quat getOrientation() const;

	void setBodyMaterial(const model::Material& material);
	const model::Material& getBodyMaterial() const;

	void reduceEnergy();
	void restoreEnergy();
	bool enoughEnergy();
	bool fullEnergy();
	bool isUpright()						{ return upright; }

	bool hasPickup()						{ return pickupEquiped; }
	std::shared_ptr<Pickup> getPickup()		{ return pickup; }
	void equipPickup(std::shared_ptr<Pickup> pickup);
	void activatePickup();
	void applyFlipImpulse();

	void applyBoost(int duration);
	void releaseBoost();

	void applyTrap();
	void releaseTrap();

	void setModelMatrix(const glm::mat4& modelMat);
	void setWheelsModelMatrix(const glm::mat4& frontLeft, const glm::mat4& frontRight, const glm::mat4& rearRight, const glm::mat4& rearLeft);
	void setPosition(const glm::vec3& position);
	void setColor(const glm::vec4 _color) { color = _color; }
	void updateSpeedometer(float deltaTime);

	float pickupTime;
	glm::vec2 currentTile;
	float energy = 1.f;
	bool suckerActive;//IMPLEMENTATION IN COLLISION DETECTION 
	bool syphonActive;//IMPLEMENTATION IN COLLISION DETECTION
	
	// driving movements act as a toggle. call the appropriate movement 
	// function to start moving then call the cooresponding stop function 
	void accelerateForward();
	void accelerateReverse();
	void brake();		
	void turnLeft();
	void turnRight();
	void hardTurn();
	void resetControls();

	void stopForward();
	void stopReverse();
	void stopBrake();
	void stopLeft();
	void stopRight();
	void stopHardTurn();

	int index;
	void render() const;
private:

	engine::teamStats::Teams team;
	int teamNum;
	glm::vec4 color;
	// orientation vectors
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	bool upright;

	glm::vec3 lastPosition;
	float speedometer;
	bool pickupEquiped = false;
	std::shared_ptr<Pickup> pickup;
	VehicleController ctrl;

	std::unique_ptr<model::Model> body;
	// Follow the order from physx. Front left, front right, rear left, rear right.
	std::array<std::unique_ptr<model::Model>, 4> wheels;
	unsigned int bodyIdx;
	unsigned int brakeLightsIdx;
	glm::vec4 brakeLightsColor;
	glm::vec4 reverseLightsColor;
	void straighten(int dir);
};
}	// namespace entity
}	// namespace hyperbright