#pragma once

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <utility>

class Camera {
public:
    Camera();
    ~Camera() = default;

    glm::mat4 lookAt();

    void processMouseMovement(GLFWwindow* window,
                              double xoffset,
                              double yoffset,
                              GLboolean constrainPitch = true);

    glm::vec3 rayCast(float length);
    glm::vec3 rayCast(float length, glm::vec3 pos);

protected:
    glm::vec3 cameraPos = glm::vec3(32.0f, 45.0f, 32.0f);
    glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float mouseSensitivity = 0.3f;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float fov = 90.0f;
};
