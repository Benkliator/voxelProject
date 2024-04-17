#include "player.h"

Player::Player() {
    worldCam = new Camera{};
    skyCam = new Camera{};
}

Player::~Player() {
    delete worldCam;
    delete skyCam;
}

void Player::moveMouse(GLFWwindow* window, float xi, float yi) {
    // view();

    worldCam->processMouseMovement(window, xi, yi);
    skyCam->processMouseMovement(window, xi, yi);
}

void Player::movePlayer(GLFWwindow* window, float dt) {
    // only worldcam because skybox moves with player
    float speed = 12.0f * dt;
    worldCam->processKeyboardInput(window, speed);
}

glm::mat4 Player::worldLook() {
    return worldCam->lookAt();
}

glm::mat4 Player::skyLook() {
    return skyCam->lookAt();
}
