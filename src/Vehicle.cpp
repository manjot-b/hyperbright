#include "Vehicle.h"
#include "Controller.h"
#include <iostream>

Vehicle::Vehicle() {}

Vehicle::Vehicle(std::shared_ptr<Model> _carModel, glm::vec3 startDir = glm::vec3(0.0f, 0.0f, -1.0f))
	: carModel(_carModel), speed(0.f), turnSpd(0.05f), forward(startDir), theta(M_PI / 2), targetAngle(M_PI / 2)
{
}

Vehicle::~Vehicle() {

}

void Vehicle::reset() {
	//Set to begining of game values

}


void Vehicle::drive(std::queue<int> inputs)
{
	int input;
	while (!inputs.empty())
	{
		input = inputs.front();
		inputs.pop();

		switch (input)
		{
		case (Controller::ACCEL):
			speed = 1.f;
			break;
		case(Controller::BRAKE):
			speed = -1.f;
			break;
		case(Controller::NO_ACC):
			speed = 0.f;
			break;
		case(Controller::TURN_LEFT):
			targetAngle += turnSpd;
			updateForward();
			break;
		case(Controller::TURN_RIGHT):
			targetAngle -= turnSpd;
			updateForward();
			break;

		}
	}
}

void Vehicle::updateForward()
{
	std::cout << "Theta: " << theta << " TargetAngle: " << targetAngle << std::endl;
	if (theta - targetAngle > 0.01f)
	{
		theta -= turnSpd;
	}
	else if (theta - targetAngle < 0.01f)
	{
		theta += turnSpd;
	}
	else
	{
		theta = targetAngle;
	}

	forward.x = cos(theta);
	forward.z = -sin(theta);
}