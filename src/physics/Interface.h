#pragma once

#include <glm/glm.hpp>

namespace hyperbright {
namespace physics {
/*
Objects of this type are dynamic objects that should be part of the PhysX engine.
*/
class IPhysical
{
public:
	/*
	Objects that have this type are either trigger volumes themselves or can collide with trigger volumes.
	Add more as the need arises.
	*/
	enum class TriggerType {
		PICKUP,
		CHARGING_STATION,
		VEHICLE,
		NONE
	};
	virtual void setModelMatrix(const glm::mat4& modelMat) = 0;
	virtual void setPosition(const glm::vec3& position) = 0;

	// TO-DO: Remove this method and have PhysX actors store pointers to whatever object
	// they need.
	virtual const char* getId() const = 0;

	virtual TriggerType getTriggerType() const { return triggerType; }
	virtual void setTriggerTrype(TriggerType type) { triggerType = type; }

protected:
	virtual ~IPhysical() {}

private:
	TriggerType triggerType;
};
}
}	// namespace hyperbright