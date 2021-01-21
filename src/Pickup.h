#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#define BATTERY 0
#define SPEED 1
#define ZAP 2
#define EMP 3
#define HIGHVOLTAGE 4
#define SLOWTRAP 5
#define SUCKER 6
#define SYPHON 7

class Pickup
{
public:
	Pickup();
	~Pickup();
	int type;
private:

};
