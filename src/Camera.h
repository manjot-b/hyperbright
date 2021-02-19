#pragma once

#include <glm/glm.hpp>

// The following code was adapted from https://learnopengl.com/Getting-started/Camera.
// An abstract camera class that processes input and calculates the corresponding
// Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Defines several possible options for camera movement. Used as abstraction to stay
	// away from window-system specific input methods
	enum Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f),
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = -90.0f, float pitch = 0.0f);

    const glm::mat4& getViewMatrix() const;
    const glm::vec3& getPosition() const;
    const glm::vec3& getDirection() const;

    void processKeyboard(Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(Movement direction, float yoffset);
    void updateCameraVectors(glm::vec3 vehPosition, glm::vec3 poi);

private:
	glm::mat4 view;
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
	glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float scrollSensitivity;

    void updateCameraVectors();
};
