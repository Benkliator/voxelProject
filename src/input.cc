#include "input.h"

Camera::Camera() {}

glm::mat4 Camera::lookAt() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::processMouseMovement(GLFWwindow* window,
                                  double xoffset,
                                  double yoffset,
                                  GLboolean constrainPitch) {
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

floatPos Camera::processKeyboardInput(GLFWwindow* window, float cameraSpeed) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -=
            glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos +=
            glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos.y += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos.y -= cameraSpeed;

    floatPos pos;
    pos.x = cameraPos.x;
    pos.y = cameraPos.y;
    pos.z = cameraPos.z;

    return pos;
}

std::pair<glm::vec3, glm::vec3> Camera::rayCast(float length) {
    return std::make_pair(cameraPos, cameraPos + (cameraFront * length));
}
