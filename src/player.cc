#include "player.h"
#include "block.h"
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <string>

Player::Player(World* w) : world{ w } {}

Player::~Player() {}

void Player::moveMouse(GLFWwindow* window, float xi, float yi) {
    processMouseMovement(window, xi, yi);
    skyCam.processMouseMovement(window, xi, yi);

    view();
}

bool Player::checkHitbox() {
    // BELOW
    ushort blockBelow =
        world->getBlock(cameraPos.x, cameraPos.y - playerHeight, cameraPos.z)
            .value_or(Block::Air << typeOffset);
    if (!isAir(blockBelow)) {
        if (ySpeed < 0.0f) {
            float blockPos = std::floor(cameraPos.y - playerHeight);
            ySpeed = 0.0f;
            onGround = true;
            cameraPos.y = std::max(cameraPos.y, blockPos + playerHeight);
        }
    } else {
        onGround = false;
    }
    // ABOVE
    ushort blockAbove = world->getBlock(cameraPos.x, cameraPos.y, cameraPos.z)
                            .value_or(Block::Air << typeOffset);
    if (!isAir(blockAbove)) {
        if (ySpeed > 0.0f) {
            float blockPos = std::floor(cameraPos.y);
            ySpeed = 0.0f;
            cameraPos.y = std::min(cameraPos.y, blockPos);
        }
    }
    // INSIDE BOTTOM
    ushort blockInBot =
        world->getBlock(cameraPos.x, cameraPos.y - 1.0f, cameraPos.z)
            .value_or(Block::Air << typeOffset);
    if (!isAir(blockInBot)) {
        return true;
    }
    // INSIDE TOP
    ushort blockInTop = world->getBlock(cameraPos.x, cameraPos.y, cameraPos.z)
                            .value_or(Block::Air << typeOffset);
    if (!isAir(blockInTop)) {
        return true;
    }
    return false;
}

void Player::movePlayer(GLFWwindow* window, float dt) {
    glm::vec3 oldCameraPos = cameraPos;
    const float cameraSpeed = 7.0f * dt;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed *
                     glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed *
                     glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -=
            glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos +=
            glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (onGround) {
            ySpeed += 5.0f;
            onGround = false;
        }
    }

    if (!onGround) {
        ySpeed -= 9.82 * dt;
    }

    if (checkHitbox()) {
        cameraPos.x = oldCameraPos.x;
        cameraPos.z = oldCameraPos.z;
    }

    cameraPos.y += ySpeed * dt;
}

void Player::selectBlock(GLFWwindow*, float, float yOffset) {
    selectedBlock = static_cast<Block::BlockType>(selectedBlock +
                                                  static_cast<int>(yOffset));
    if (selectedBlock <= Block::Air) {
        selectedBlock =
            static_cast<Block::BlockType>(Block::NUM_BLOCKTYPES - 1);
    }
    if (selectedBlock >= Block::NUM_BLOCKTYPES) {
        selectedBlock = static_cast<Block::BlockType>(Block::Air + 1);
    }
}

// Make less scuffed in future lol
void Player::breakBlock() {
    unsigned xChunk = viewBlock.x - (viewBlock.x % 16);
    unsigned zChunk = viewBlock.z - (viewBlock.z % 16);

    world->getChunk(xChunk, 0, zChunk)
        ->removeBlock(viewBlock.x % 16, viewBlock.y, viewBlock.z % 16);

    view();
}

// NOTE: this works but is sub-optimal
void Player::placeBlock() {
    auto [from, to] = rayCast(5.0f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);

    float it = 1.0f;
    while (!isAir(world->getChunk(xChunk, 0, zChunk)
                      ->getBlock(xBlock % 16, yBlock, zBlock % 16)) &&
           it <= 4.0f) {
        auto [from, to] = rayCast(5.0f - it);

        xBlock = unsigned(std::round(to.x));
        yBlock = unsigned(std::round(to.y));
        zBlock = unsigned(std::round(to.z));

        xChunk = xBlock - (xBlock % 16);
        zChunk = zBlock - (zBlock % 16);
        it++;
    }
    if (it != 4) {
        auto [from, to] = rayCast(5.0f - it);
        world->getChunk(xChunk, 0, zChunk)
            ->placeBlock(selectedBlock, xBlock % 16, yBlock, zBlock % 16);
        view();
    }
}

glm::mat4 Player::worldLook() {
    return this->lookAt();
}

glm::mat4 Player::skyLook() {
    return skyCam.lookAt();
}

void Player::draw() {
    std::string playerPosStr{};
    playerPosStr += std::to_string(cameraPos.x);
    playerPosStr += " : ";
    playerPosStr += std::to_string(cameraPos.y);
    playerPosStr += " : ";
    playerPosStr += std::to_string(cameraPos.z);
    hud.renderText(
        playerPosStr, 5.0f, 0.0f, 0.5f, glm::vec3{ 1.0f, 1.0f, 1.0f });

    hud.renderText("Selected block: " + blockToString(selectedBlock),
                   5.0f,
                   20.0f,
                   0.5f,
                   glm::vec3{ 1.0f, 1.0f, 1.0f });
}

// NOTE: better raycasting?
void Player::view() {
    auto [from, to] = rayCast(1.0f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);
    float it = 1.0f;
    while (isAir(world->getChunk(xChunk, 0, zChunk)
                     ->getBlock(xBlock % 16, yBlock, zBlock % 16)) &&
           it <= 4.0f) {
        auto [from, to] = rayCast(1.0f + it);

        xBlock = unsigned(std::round(to.x));
        yBlock = unsigned(std::round(to.y));
        zBlock = unsigned(std::round(to.z));

        xChunk = xBlock - (xBlock % 16);
        zChunk = zBlock - (zBlock % 16);
        it++;
    }
    Chunk* prevChunk = world->getChunk(
        viewBlock.x - (viewBlock.x % 16), 0, viewBlock.z - (viewBlock.z % 16));
    // Unhighlight viewblock
    if (prevChunk) {
        prevChunk->unhighlightBlock(
            viewBlock.x % 16, viewBlock.y, viewBlock.z % 16);
        prevChunk->clearMesh();
        prevChunk->generateMesh();
    }
    Chunk* temp = world->getChunk(xChunk, 0, zChunk);
    if (!isAir(temp->getBlock(xBlock % 16, yBlock, zBlock % 16))) {
        // Highlight new viewblock
        temp->highlightBlock(xBlock % 16, yBlock, zBlock % 16);
        // Reload mesh
        temp->clearMesh();
        temp->generateMesh();
        // Assign new viewblock
        viewBlock = glm::uvec3(xBlock, yBlock, zBlock);
    }
}
