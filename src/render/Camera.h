#pragma once
#include "entity/Vehicle.h"

#include <glm/glm.hpp>
#include <memory>

namespace hyperbright {
namespace render {
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
    void updateCameraVectors(glm::vec3 vehPosition, glm::vec3 poi); // static camera behind vehicle
    void updateCameraVectors(std::shared_ptr<entity::Vehicle>& player, float deltaTime); // dynamic camera on boom 
    void initCameraBoom(glm::vec3 position, glm::vec3 direction);
    void setConfigs(float camHeight, float camVelCoef, float camRestLen, float camSwStr, float poiHeight, float poiDepth);

private:
    glm::mat4 view;
    // camera Attributes
    glm::vec3 position;
    glm::vec3 direction;
    // orientation
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // camera/poi positioning
    float camHeight = 2.5f;
    float camVelocityCoeficient = 4.f;
    float camRestLength = 7.5f;
    float camSwingStrength = 0.07f;
    float poiHeight = -0.4f;
    float poiDepth = 2.f;
    float panL = 0.f;
    float panR = 0.f;
    float panU = 0.f;
    float panD = 0.f;

    struct BoomArm {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 velocity;
        float currentLength;
        float restingLength;
    }boomArm;

    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float scrollSensitivity;

    void updateCameraVectors();
public:
    void panLeft(float p)  { panL = -p; }
    void panRight(float p) { panR =  p; }
    void panUp(float p)    { panU = -p; }
    void panDown(float p)  { panD =  p; }
};
}   // namespace render
}   // namespace hyperbright