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

void Camera::initCameraBoom(glm::vec3 position, glm::vec3 direction)
{
	
	boomArm.velocity = glm::vec3(0.f);
	boomArm.restingLength = camRestLength;
	boomArm.currentLength = camRestLength;
	boomArm.position = glm::vec3(position.x, camHeight, position.z);
	boomArm.direction = glm::normalize(direction);
}

void Camera::updateCameraVectors(std::shared_ptr<entity::Vehicle>& player, float deltaTime)
{
	glm::vec3 pDir = player->getDirection();	// direction of the vehicle
	glm::vec3 pPos = player->getPosition();		// position of the vehilce

	glm::vec3 poi = pPos + pDir * poiDepth;	// poi of interest in front 
	poi.y += poiHeight;						// and slightly below the vehicle

	// orientation vectors
	front = glm::normalize(poi - boomArm.position);	
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	// swing the camera to the back of the vehicle
	glm::vec3 betwP_B = (poi - pDir * camRestLength) - boomArm.position;					// vector from the resting position to the current boom position
	boomArm.position += camSwingStrength * glm::length(betwP_B) * glm::normalize(betwP_B);	// close that gap proportional to it's distance

	// semi-implicit Euler integration to accelerate the boom towards the poi
	boomArm.currentLength = glm::length(poi - boomArm.position);							// length between the poi and boom position
	boomArm.velocity = (boomArm.currentLength - boomArm.restingLength) * front * camVelocityCoeficient;	// update velocity proportional to the distance from rest * by some catch up factor
	boomArm.position = boomArm.position + boomArm.velocity * deltaTime;						// update position based on new velocity * a small change in time
	boomArm.position.y = camHeight;		// Always keep the camera at the same height

	// TO-DO: alter the up direction based on the turning radius

	boomArm.direction = boomArm.position + front;	// update direction based on booms position and direction to poi
	view = glm::lookAt(boomArm.position, boomArm.direction, up);
}

void Camera::setConfigs(float _camHeight, float _camVelCoef, float _camRestLen, float _camSwStr, float _poiHeight, float _poiDepth) {
	camHeight = _camHeight;
	camVelocityCoeficient = _camVelCoef;
	camRestLength = _camRestLen;
	camSwingStrength = _camSwStr;
	poiHeight = _poiHeight;
	poiDepth = _poiDepth;
}
}   // namespace render
}   // namespace hyperbright