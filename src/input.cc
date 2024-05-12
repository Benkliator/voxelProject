#include "input.h"

Camera::Camera() {}

glm::mat4 Camera::lookAt() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::processMouseMovement(GLFWwindow* window,
                                  double xoffset,
                                  double yoffset,
                                  GLboolean constrainPitch) {
    (void)window; // Unused
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraFront);
}

glm::vec3 Camera::rayCast(float length) {
    return cameraPos + (cameraFront * length);
}

glm::vec3 Camera::rayCast(float length, glm::vec3 pos) {
    return pos - (cameraFront * length);
}
