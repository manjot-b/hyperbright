#include "Camera.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	position(position),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	worldUp(up),
	yaw(yaw),
	pitch(pitch),
	movementSpeed(6.f),
	mouseSensitivity(0.2f),
	scrollSensitivity(0.5f)
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
	float newX = vehPosition.x - (poi.x - vehPosition.x)*4.f;
	float newZ = vehPosition.z - (poi.z - vehPosition.z)*4.f;
	
	position = { newX, 2.f, newZ };
	front = glm::normalize(poi - position);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	direction = position + front;
	view = glm::lookAt(position, direction, up);
}
