#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

#include "model/Model.h"
#include "entity/Arena.h"
#include "render/IRenderable.h"
#include "opengl-helper/Shader.h"
#include "physics/Interface.h"
#include "engine/TeamStats.h"

#define SYPHON 4
#define SLOWTRAP 3
#define SPEED 2
#define ZAP 1
#define EMP 0


#define SUCKER 6

namespace hyperbright {
namespace entity {
	class Vehicle;//Forward declaration
	class PickupManager;
}	// namespace entity

namespace entity {
class Pickup : public render::IRenderable, public physics::IPhysical
{
public:
	Pickup();
	Pickup(const std::shared_ptr<openGLHelper::Shader>& shader);
	~Pickup();
	//int getType() { return type; }
	int pickupNumber;
	Pickup(int puNum, int pickupType, std::shared_ptr<PickupManager> pickupManager, const std::shared_ptr<openGLHelper::Shader>& shader);
	//void activate(std::vector<std::shared_ptr<entity::Vehicle>>* vehicles, int indexOfFirstPlace);
	void activate(std::vector<std::shared_ptr<entity::Vehicle>>* _vehicles);
	//void deactivate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace);
	void deactivate(std::vector<std::shared_ptr<entity::Vehicle>>* vehicles, std::shared_ptr<entity::Arena> arena);
	//void initialCollision(std::shared_ptr<Vehicle> vehicle);
	bool active;
	bool timeRemaining();
	//Position
	void render() const;
	void renderShadow(const std::shared_ptr<openGLHelper::Shader>& shadowShader) const;
	void renderMiniMap() const;
	void animate(float deltaSec);

	void use(engine::teamStats::Teams team, int teamNum);//VEHICLES CALL THIS FUNCTION

	void setModelMatrix(const glm::mat4& modelMat);
	void setPosition(const glm::vec3& position);

	void setArenaLocation(glm::vec3 _arenaLocation, std::optional<glm::vec2> tileLocation);
	glm::vec3 getArenaLocation() { return arenaLocation; }
	glm::vec2 tile;
	glm::vec2 trapTile;
	bool beingCarried;
	//int usedByIndex;
	int type;
private:
	int whoWasZapped;
	engine::teamStats::Teams zapOldTeam;
	engine::teamStats::Teams usedByTeam;

	int usedByTeamNum;

	//int type;
	float pickupTime;
	float pickUpStartTime;
	//glm::vec4 zapOldColor;//CHANGE TYPE LATER
	bool slowTrapActive;
	float speedOldMax;
	glm::vec3 arenaLocation;
	//Vehicle* carriedBy;
	std::shared_ptr<PickupManager> pickupManager;
	std::shared_ptr<model::Model> model;
};
}	// namespace entity
}	// namespace hyperbright