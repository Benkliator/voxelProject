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
    auto [from, to] = worldCam->rayCast(1.0f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);

    world->getChunk(xChunk, 0, zChunk)
         ->removeBlock(xBlock % 16, yBlock, zBlock % 16);
}

void Player::placeBlock() {
    auto [from, to] = worldCam->rayCast(1.0f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);

    world->getChunk(xChunk, 0, zChunk)
         ->placeBlock(xBlock % 16, yBlock, zBlock % 16);
}

glm::mat4 Player::worldLook() {
    return worldCam->lookAt();
}

glm::mat4 Player::skyLook() {
    return skyCam->lookAt();
}
