#include "Camera.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

namespace hyperbright {
namespace render {
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	position(position),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	worldUp(up),
	yaw(yaw),
	pitch(pitch),
	movementSpeed(6.f),
	mouseSensitivity(0.2f),
	scrollSensitivity(3.f)
{
	updateCameraVectors();	
}


void Camera::processKeyboard(Movement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == LEFT)
		position -= right * velocity;
	if (direction == RIGHT)
		position += right * velocity;
	if (direction == UP)
		position += up * velocity;
	if (direction == DOWN)
		position -= up * velocity;

	updateCameraVectors();
}

void Camera::processMouseMovement(float xoffset, float yoffset)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::processMouseScroll(Movement direction, float yoffset)
{
	float velocity = scrollSensitivity * yoffset;
	if (direction == FORWARD)
		position += front * velocity;
	if (direction == BACKWARD)
		position -= front * velocity;

	updateCameraVectors();
}

const glm::mat4& Camera::getViewMatrix() const
{
	return view;
}

const glm::vec3& Camera::getPosition() const
{
	return position;
}

const glm::vec3& Camera::getDirection() const
{
	return direction;
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 frontNew;
	frontNew.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontNew.y = sin(glm::radians(pitch));
	frontNew.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(frontNew);
	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));

	direction = position + front;
	view = glm::lookAt(position, direction,  up);
}

void Camera::updateCameraVectors(glm::vec3 vehPosition, glm::vec3 poi)
{
	float belowForward = 0.4f;
	float aboveShoulder = 2.f;
	float behindShoulder = 4.5;

	poi.y -= belowForward;
	float newX = vehPosition.x - (poi.x - vehPosition.x)* behindShoulder;
	float newZ = vehPosition.z - (poi.z - vehPosition.z)* behindShoulder;
	
	position = { newX, aboveShoulder, newZ };
	front = glm::normalize(poi - position);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	direction = position + front;
	view = glm::lookAt(position, direction, up);
}

float const aboveShoulder = 2.f;
float const belowForward = 0.4f;
float const aheadVehicle = 2.f;
float const catchUp = 10.f;
//float const straighten = 1.f;
//float deltaTheta = 0.f;
//float theta = 0.f;

void Camera::initCameraBoom(glm::vec3 position, glm::vec3 direction)
{
	float testLength = 5.5f;
	boomArm.velocity = glm::vec3(0.f);
	boomArm.restingLength = testLength;
	boomArm.currentLength = testLength;
	boomArm.position = glm::vec3(position.x, aboveShoulder, position.z);
	boomArm.direction = glm::normalize(direction);
}

void Camera::updateCameraVectors(std::shared_ptr<entity::Vehicle>& player, float deltaTime)
{
	glm::vec3 pDir = player->getDirection();
	glm::vec3 pPos = player->getPosition();

	glm::vec3 poi = pPos + pDir * aheadVehicle;
	poi.y -= belowForward;

	front = glm::normalize(poi - boomArm.position);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	/*
	float dotF_D = glm::dot(front, pDir);
	float deltaTheta = 0.f;
	if (dotF_D > 0.1f || dotF_D < -0.1f) {
		deltaTheta = (1.f - dotF_D) * straighten;
	}

	glm::vec3 perpP_C = glm::cross(front, pDir);

	if (deltaTheta != 0.f) {
		if (perpP_C.y > 0.f) {
			glm::mat4 rotM = glm::rotate(glm::mat4(1.f), glm::radians(deltaTheta), up);
			front = glm::vec3(rotM * glm::vec4(front, 1.f));
		}
		else if (perpP_C.y < 0.f) {
			glm::mat4 rotM = glm::rotate(glm::mat4(1.f), glm::radians(-deltaTheta), up);
			front = glm::vec3(rotM * glm::vec4(front, 1.f));
		}
	}*/

	boomArm.currentLength = glm::length(poi - boomArm.position);
	boomArm.velocity = (boomArm.currentLength - boomArm.restingLength) * front * catchUp;
	boomArm.position = boomArm.position + boomArm.velocity * deltaTime;
	boomArm.position.y = aboveShoulder;

	position = boomArm.position;
	direction = position + front;

	//std::cout << "Position:  " << position.x << " " << position.y << " " << position.z << std::endl;
	//std::cout << "Direction: " << direction.x << " " << direction.y << " " << direction.z << "\n" << std::endl;
	view = glm::lookAt(position, direction, up);

}
}   // namespace render
}   // namespace hyperbright