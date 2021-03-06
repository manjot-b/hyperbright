#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

#include "model/Model.h"
#include "render/Renderer.h"

#define BATTERY 0
#define SPEED 1
#define ZAP 2
#define EMP 3
#define HIGHVOLTAGE 4
#define SLOWTRAP 5
#define SUCKER 6
#define SYPHON 7

namespace hyperbright {
namespace entity {
	class Vehicle;//Forward declaration
	class PickupManager;
}	// namespace entity

namespace entity {
class Pickup : public render::Renderer::IRenderable
{
public:
	Pickup();
	~Pickup();
	Pickup(int pickupType, std::shared_ptr<PickupManager> pickupManager );
	void activate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace);
	void deactivate(Vehicle vehicles[], int indexOfActivator, int indexOfFirstPlace);
	void initialCollision(std::shared_ptr<Vehicle> vehicle);
	bool active;
	//bool beingCarried;
	bool timeRemaining();
	//Position
	void render(const openGLHelper::Shader& shader) const;
	void animate(float deltaSec);
private:
	int type;
	float pickupTime;
	float pickUpStartTime;
	glm::vec4 zapOldColor;//CHANGE TYPE LATER
	bool slowTrapActive;
	float speedOldMax;
	//Vehicle* carriedBy;
	std::shared_ptr<PickupManager> pickupManager;
	std::shared_ptr<model::Model> model;
};
}	// namespace entity
}	// namespace hyperbright