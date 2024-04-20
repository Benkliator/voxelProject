#include "player.h"

#include <iostream>

Player::Player(World* w) : world{ w } {
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
    pos = worldCam->processKeyboardInput(window, speed);
}

void Player::breakBlock() {
    std::cout << "Breaking block!" << std::endl;
    auto [from, to] = worldCam->rayCast(5.0f);
    std::cout << from.x << ' ' << to.x << std::endl;
    int xBlock = int(to.x);
    int yBlock = int(to.y);
    int zBlock = int(to.z);
}

void Player::placeBlock() {
    std::cout << "Placing block!" << std::endl;
}

glm::mat4 Player::worldLook() {
    return worldCam->lookAt();
}

glm::mat4 Player::skyLook() {
    return skyCam->lookAt();
}
