#pragma once

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "position.h"

// TODO: Put functions functionality in a sepparate cc file, I am too lazy.
class Camera {
public:
    Camera();

    ~Camera() = default;

    glm::mat4 lookAt();

    void processMouseMovement(GLFWwindow* window,
                              double xoffset,
                              double yoffset,
                              GLboolean constrainPitch = true);

    floatPos processKeyboardInput(GLFWwindow* window, float deltaTime);

private:
    glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float mouseSensitivity = 0.3f;
    bool firstMouse = true;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float fov = 90.0f;
};
