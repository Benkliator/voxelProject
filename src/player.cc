#include "player.h"
#include "block.h"
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <string>

Player::Player(World* w, glm::vec3 pos) : world{ w } {
    cameraPos = pos;

    unsigned xChunk = (unsigned)cameraPos.x - ((unsigned)(cameraPos.x) % 16);
    unsigned zChunk = (unsigned)cameraPos.z - ((unsigned)(cameraPos.z) % 16);
    chunkPos = glm::uvec3(xChunk, 0, zChunk); 
}

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
    view();

    unsigned xChunk = (unsigned)cameraPos.x - ((unsigned)(cameraPos.x) % 16);
    unsigned zChunk = (unsigned)cameraPos.z - ((unsigned)(cameraPos.z) % 16);
    glm::uvec3 tempChunkPos = glm::uvec3(xChunk, 0, zChunk); 
    if (tempChunkPos.x != chunkPos.x || tempChunkPos.z != chunkPos.z) {
        world->reloadChunksAround(xChunk, 0, zChunk);
        chunkPos = tempChunkPos;
    }
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
    auto chunkOpt = world->getChunk(xChunk, 0, zChunk);
    if (chunkOpt.has_value()) {
        Chunk& chunk = chunkOpt.value().get();
        chunk.removeBlock(viewBlock.x % 16, viewBlock.y, viewBlock.z % 16);
    }

    view();
}

// NOTE: this works but for future make the block face placig more intuitive
// i.e somehow figure out which block is being looked at.
void Player::placeBlock() {
    unsigned xChunk = viewBlock.x - (viewBlock.x % 16);
    unsigned zChunk = viewBlock.z - (viewBlock.z % 16);

    auto tempChunkOpt = world->getChunk(xChunk, 0, zChunk);
    if (tempChunkOpt.has_value()) {
        Chunk& tempChunk = tempChunkOpt.value().get();
        tempChunk.placeBlock(
            selectedBlock, viewBlock.x % 16, viewBlock.y + 1, viewBlock.z % 16);
    }

    view();
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

// TODO: better raycasting?
void Player::view() {
    auto [from, to] = rayCast(1.0f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);
    float it = 1.0f;
    while (it <= 4.0f) {
        auto chunkOpt = world->getChunk(xChunk, 0, zChunk);
        if (chunkOpt.has_value()) {
            Chunk& chunk = chunkOpt.value().get();
            if (!isAir(chunk.getBlock(xBlock % 16, yBlock, zBlock % 16))) {
                break;
            }
        }
        auto [from, to] = rayCast(1.0f + it);

        xBlock = unsigned(std::round(to.x));
        yBlock = unsigned(std::round(to.y));
        zBlock = unsigned(std::round(to.z));

        xChunk = xBlock - (xBlock % 16);
        zChunk = zBlock - (zBlock % 16);
        it++;
    }
    auto prevChunkOpt = world->getChunk(
        viewBlock.x - (viewBlock.x % 16), 0, viewBlock.z - (viewBlock.z % 16));
    // Unhighlight viewblock
    if (prevChunkOpt.has_value()) {
        Chunk& prevChunk = prevChunkOpt.value().get();
        prevChunk.unhighlightBlock(
            viewBlock.x % 16, viewBlock.y, viewBlock.z % 16);
        prevChunk.clearMesh();
        prevChunk.generateMesh();
    }
    auto tempChunkOpt = world->getChunk(xChunk, 0, zChunk);
    if (tempChunkOpt.has_value()) {
        Chunk& tempChunk = tempChunkOpt.value().get();
        if (!isAir(tempChunk.getBlock(xBlock % 16, yBlock, zBlock % 16))) {
            // Highlight new viewblock
            tempChunk.highlightBlock(xBlock % 16, yBlock, zBlock % 16);
            // Reload mesh
            tempChunk.clearMesh();
            tempChunk.generateMesh();
            // Assign new viewblock
            viewBlock = glm::uvec3(xBlock, yBlock, zBlock);
        }
    }
}
