#include "player.h"
#include "block.h"
#include "game.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <string>
#include <vector>

Player::Player(World* w, glm::vec3 pos) : world{ w } {
    cameraPos = pos;
    prevCameraPos = pos;
    nextCameraPos = pos;

    unsigned xChunk = static_cast<unsigned>(cameraPos.x) -
                      (static_cast<unsigned>(cameraPos.x) % 16);
    unsigned zChunk = static_cast<unsigned>(cameraPos.z) -
                      (static_cast<unsigned>(cameraPos.z) % 16);
    chunkPos = glm::uvec3(xChunk, 0, zChunk);
    velocity = glm::vec3(0.0, 0.0, 0.0);
}

Player::~Player() {}

void Player::moveMouse(GLFWwindow* window, float xi, float yi) {
    processMouseMovement(window, xi, yi);
    skyCam.processMouseMovement(window, xi, yi);

    view();
}

void Player::checkCollision() {

    // SIDES
    bool collision = false;
    /*
    for (int height = 0; height < playerHeight; ++height) {
        ushort checkBlock;
        const std::vector<float> checkBlocksX = {
            std::round(nextCameraPos.x + playerWidth),
            std::round(nextCameraPos.x - playerWidth)
        };

        const std::vector<float> checkBlocksZ = {
            std::round(nextCameraPos.z + playerWidth),
            std::round(nextCameraPos.z - playerWidth)
        };

        if (abs(velocity.x) > abs(velocity.z)) {
            for (auto zPos : checkBlocksZ) {
                if (velocity.x > 0) {
                    int xPos = std::round(nextCameraPos.x + playerWidth);
                    checkBlock = world->getBlock(xPos,
    std::round(nextCameraPos.y - height), zPos).value_or(Block::Air <<
    typeOffset); if (!isAir(checkBlock)) { collision = true; velocity.x = 0;
                        float wallPos = xPos - 0.5f - playerWidth;
                        nextCameraPos.x = wallPos;
                        break;
                    }
                } else if (velocity.x < 0) {
                    int xPos = std::round(nextCameraPos.x - playerWidth);
                    checkBlock = world->getBlock(xPos,
    std::round(nextCameraPos.y - height), zPos).value_or(Block::Air <<
    typeOffset); if (!isAir(checkBlock)) { collision = true; velocity.x = 0;
                        float wallPos = xPos + 0.5f + playerWidth;
                        nextCameraPos.x = wallPos;
                        break;
                    }
                }
            }
        } else {
            for (auto xPos : checkBlocksX) {
                if (velocity.z > 0) {
                    int zPos = std::round(nextCameraPos.z + playerWidth);
                    checkBlock = world->getBlock(xPos,
    std::round(nextCameraPos.y - height), zPos).value_or(Block::Air <<
    typeOffset); if (!isAir(checkBlock)) { collision = true; velocity.z = 0;
                        float wallPos = zPos - 0.5f - playerWidth;
                        nextCameraPos.z = wallPos;
                        break;
                    }
                } else if (velocity.z < 0) {
                    int zPos = std::round(nextCameraPos.z - playerWidth);
                    checkBlock = world->getBlock(xPos,
    std::round(nextCameraPos.y - height), zPos).value_or(Block::Air <<
    typeOffset); if (!isAir(checkBlock)) { collision = true; velocity.z = 0;
                        float wallPos = zPos + 0.5f + playerWidth;
                        nextCameraPos.z = wallPos;
                        break;
                    }
                }
            }
        }

        if (collision) {
            break;
        }

    }
    */

    const std::vector<glm::vec3> checkBlocks = {
        { std::round(nextCameraPos.x - playerWidth),
          std::round(nextCameraPos.y - playerHeight),
          std::round(nextCameraPos.z - playerWidth) },
        { std::round(nextCameraPos.x - playerWidth),
          std::round(nextCameraPos.y - playerHeight),
          std::round(nextCameraPos.z + playerWidth) },
        { std::round(nextCameraPos.x + playerWidth),
          std::round(nextCameraPos.y - playerHeight),
          std::round(nextCameraPos.z + playerWidth) },
        { std::round(nextCameraPos.x + playerWidth),
          std::round(nextCameraPos.y - playerHeight),
          std::round(nextCameraPos.z - playerWidth) },
    };

    // ABOVE
    for (auto& blockPos : checkBlocks) {
        ushort checkBlock;
        checkBlock = world
                         ->getBlock(blockPos.x,
                                    std::ceil(blockPos.y + playerHeight),
                                    blockPos.z)
                         .value_or(Block::Air << typeOffset);
        if (!isAir(checkBlock) && velocity.y > 0) {
            velocity.y = 0;
            float roofPos = blockPos.y;
            nextCameraPos.y = roofPos + playerHeight;
            break;
        }
    }

    // BELOW
    onGround = false;
    for (auto& blockPos : checkBlocks) {
        ushort checkBlock;
        checkBlock = world->getBlock(blockPos.x, blockPos.y, blockPos.z)
                         .value_or(Block::Air << typeOffset);
        if (!isAir(checkBlock)) {
            velocity.y = 0;
            onGround = true;
            float floorPos = blockPos.y + 0.5f;
            nextCameraPos.y = floorPos + playerHeight;
            break;
        }
    }
}

void Player::commandHandler() {
    if (line.starts_with("/craft ")) {
        std::string target = line.substr(7);
        std::cout << "crafting: " << target << std::endl;
    }
}

void Player::movePlayer(GLFWwindow* window) {
    float horizontalSpeed = 1.0;
    float verticalSpeed = 0.5;

    float sprintBoost = 0.3;
    float gravitySpeed = 0.098;

    auto enterKey = glfwGetKey(window, GLFW_KEY_ENTER);
    if (enterKey == GLFW_PRESS) {
        if (!chatKeyPressed) {
            typing = !typing;
            if (typing) {
                auto chatCallback = [](GLFWwindow* w, uint k) {
                    static_cast<Game*>(glfwGetWindowUserPointer(w))
                        ->player->line += k;
                };
                glfwSetCharCallback(window, chatCallback);
            } else {
                glfwSetCharCallback(window, nullptr);
                commandHandler();
                chatLog.emplace_back(std::move(line));
                line = std::string{}; // reset
            }
            chatKeyPressed = true;
        }
    } else {
        chatKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        velocity += glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        velocity -= glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        velocity -= glm::normalize(
            glm::cross(glm::vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        velocity += glm::normalize(
            glm::cross(glm::vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        switch (mode) {
        case survival: {
            if (onGround) {
                velocity.y += verticalSpeed;
                onGround = false;
            }
        } break;
        case creative: {
            velocity.y += verticalSpeed;
            onGround = false;
        } break;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        switch (mode) {
        case survival: {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                velocity +=
                    glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z)) *
                    sprintBoost;
            }
        } break;
        case creative: {
            velocity.y -= verticalSpeed;
        } break;
        }
    }

    auto modeswitchKey = glfwGetKey(window, GLFW_KEY_G);
    if (modeswitchKey == GLFW_PRESS) {
        if (!modeKeyPressed) {
            modeKeyPressed = false;
            velocity.y = 0.0f;
            switch (mode) {
            case survival: {
                mode = creative;
            }; break;
            case creative: {
                mode = survival;
            } break;
            }
            modeKeyPressed = true;
        }
    } else if (modeswitchKey == GLFW_RELEASE) {
        modeKeyPressed = false;
    }

    switch (mode) {
    case survival: {
        if (onGround) {
            velocity.x /= 5.0;
            velocity.z /= 5.0;
        } else {
            velocity.y -= gravitySpeed;
            velocity.x /= 4.8;
            velocity.z /= 4.8;
        }
    } break;
    case creative: {
        velocity.y /= 1.8;
        velocity.x /= 2.8;
        velocity.z /= 2.8;
    } break;
    }

    prevCameraPos = nextCameraPos;
    nextCameraPos += velocity;
    switch (mode) {
    case survival: {
        checkCollision();
    } break;
    case creative: {
    } break;
    }
}

void Player::timeStep(float dt) {
    cameraPos += (nextCameraPos - cameraPos) * dt;
}

void Player::checkChunk() {
    unsigned xChunk = static_cast<unsigned>(cameraPos.x) -
                      (static_cast<unsigned>(cameraPos.x) % 16);
    unsigned zChunk = static_cast<unsigned>(cameraPos.z) -
                      (static_cast<unsigned>(cameraPos.z) % 16);
    glm::uvec3 tempChunkPos = glm::uvec3(xChunk, 0, zChunk);
    if (tempChunkPos != chunkPos) {
        world->reloadChunksAround(xChunk, 0, zChunk);
    }
    chunkPos = tempChunkPos;
    view();
}

void Player::selectBlock(GLFWwindow*, float yOffset) {
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
    const unsigned chunkMask = 0b1111;
    unsigned xChunk = viewBlock.x & ~chunkMask;
    unsigned zChunk = viewBlock.z & ~chunkMask;
    unsigned xOff = viewBlock.x & chunkMask;
    unsigned zOff = viewBlock.z & chunkMask;
    auto chunkOpt = world->getChunkFast(xChunk, 0, zChunk);
    if (chunkOpt.has_value()) {
        Chunk& chunk = chunkOpt.value().get();
        unsigned block = chunk.getBlock(xOff, viewBlock.y, zOff);
        if (!isAir(block)) {
            inventory[blockType(block)]++;
        }
        chunk.removeBlock(xOff, viewBlock.y, zOff);
    }

    std::cout << "-- BLOCKS --" << std::endl;
    for (auto& [k, v] : inventory) {
        std::cout << blockToString(k) << ": " << v << std::endl;
    }

    view();
}

// NOTE: this works but for future make the block face placig more intuitive
// i.e somehow figure out which block is being looked at.
void Player::placeBlock() {
    const unsigned chunkMask = 0b1111;
    unsigned xChunk = viewBlock.x & ~chunkMask;
    unsigned zChunk = viewBlock.z & ~chunkMask;
    unsigned xOff = viewBlock.x & chunkMask;
    unsigned zOff = viewBlock.z & chunkMask;

    auto tempChunkOpt = world->getChunkFast(xChunk, 0, zChunk);
    if (tempChunkOpt.has_value()) {
        Chunk& tempChunk = tempChunkOpt.value().get();
        if (isAir(tempChunk.getBlock(xOff, viewBlock.y, zOff))) {
            return;
        }
    }

    glm::uvec3 placePos = viewBlock;
    switch (selectedFace) {
    case Block::Top: {
        placePos.y++;
    } break;
    case Block::Bottom: {
        placePos.y--;
    } break;
    case Block::Right: {
        placePos.x++;
    } break;
    case Block::Left: {
        placePos.x--;
    } break;
    case Block::Front: {
        placePos.z++;
    } break;
    case Block::Back: {
        placePos.z--;
    } break;
    }

    xChunk = placePos.x - (placePos.x % 16);
    zChunk = placePos.z - (placePos.z % 16);

    glm::uvec3 roundedPos = glm::uvec3(std::round(cameraPos.x),
                                       std::round(cameraPos.y),
                                       std::round(cameraPos.z));
    if (placePos == roundedPos ||
        placePos == glm::uvec3(roundedPos.x, roundedPos.y - 1, roundedPos.z)) {
        return;
    }

    tempChunkOpt = world->getChunkFast(xChunk, 0, zChunk);
    if (tempChunkOpt.has_value()) {
        Chunk& tempChunk = tempChunkOpt.value().get();
        tempChunk.placeBlock(
            selectedBlock, placePos.x % 16, placePos.y, placePos.z % 16);
    }

    view();
}

glm::mat4 Player::worldLook() {
    return this->lookAt();
}

glm::mat4 Player::skyLook() {
    return skyCam.lookAt();
}

void Player::draw(std::string& fps) {
    // FPS
    hud.renderText(
        fps, 5.0f, SCR_HEIGHT - 40.0f, 1.0f, glm::vec3{ 1.0f, 1.0f, 1.0f });
    // Position
    std::string playerPosStr{};
    playerPosStr += " x: " + std::to_string(static_cast<int>(cameraPos.x));
    playerPosStr +=
        " y: " + std::to_string(static_cast<int>(cameraPos.y - playerHeight));
    playerPosStr += " z: " + std::to_string(static_cast<int>(cameraPos.z));
    hud.renderText(
        playerPosStr, 5.0f, 0.0f, 0.5f, glm::vec3{ 1.0f, 1.0f, 1.0f });
    // Selected block
    hud.renderText("Selected block: " + blockToString(selectedBlock),
                   5.0f,
                   20.0f,
                   0.5f,
                   glm::vec3{ 1.0f, 1.0f, 1.0f });
    // Mode
    std::string modeStr = "Mode: ";
    switch (mode) {
    case survival: {
        modeStr += "survival";
    } break;
    case creative: {
        modeStr += "creative";
    } break;
    }
    hud.renderText(modeStr, 5.0f, 40.0f, 0.5f, glm::vec3{ 1.0f, 1.0f, 1.0f });
    // Crosshair
    hud.renderText("*",
                   SCR_WIDTH / 2.0f,
                   SCR_HEIGHT / 2.0f,
                   0.5f,
                   glm::vec3{ 1.0f, 1.0f, 1.0f });

    // Chat log
    // TODO: Trim beginning when log gets off screen
    for (size_t i = 0; i < chatLog.size(); i++) {
        hud.renderText(chatLog[i],
                       0,
                       SCR_HEIGHT / 2.0f + i * 20.0f,
                       0.5f,
                       glm::vec3{ 1.0f, 1.0f, 1.0f });
    }

    // Current chat line
    hud.renderText(
        line, SCR_WIDTH / 2.0f, 0, 0.5f, glm::vec3{ 1.0f, 1.0f, 1.0f });
}

void Player::view() {
    glm::vec3 to = rayCast(0.02f);

    unsigned xBlock = static_cast<unsigned>(std::round(to.x));
    unsigned yBlock = static_cast<unsigned>(std::round(to.y));
    unsigned zBlock = static_cast<unsigned>(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);
    float it = 0.1f;
    while (it <= 4.0f) {
        auto chunkOpt = world->getChunkSlow(xChunk, 0, zChunk);
        if (chunkOpt.has_value()) {
            Chunk& chunk = chunkOpt.value().get();
            if (!isAir(chunk.getBlock(xBlock % 16, yBlock, zBlock % 16))) {
                break;
            }
        }
        to = rayCast(0.1f + it);

        xBlock = static_cast<unsigned>(std::round(to.x));
        yBlock = static_cast<unsigned>(std::round(to.y));
        zBlock = static_cast<unsigned>(std::round(to.z));

        xChunk = xBlock - (xBlock % 16);
        zChunk = zBlock - (zBlock % 16);
        it += 0.1;
    }
    auto prevChunkOpt = world->getChunkSlow(
        viewBlock.x - (viewBlock.x % 16), 0, viewBlock.z - (viewBlock.z % 16));
    // Unhighlight viewblock
    if (prevChunkOpt.has_value()) {
        Chunk& prevChunk = prevChunkOpt.value().get();
        prevChunk.unhighlightBlock(
            viewBlock.x % 16, viewBlock.y, viewBlock.z % 16);
        prevChunk.clearMesh();
        prevChunk.generateMesh();
    }
    auto tempChunkOpt = world->getChunkFast(xChunk, 0, zChunk);
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

            if (abs(to.x - viewBlock.x - 0.5) > 0.9f &&
                abs(to.x - viewBlock.x) <= 1.0f) {
                selectedFace = Block::Left;
            } else if (abs(to.x - viewBlock.x - 0.5) < 0.1f) {
                selectedFace = Block::Right;
            }

            if (abs(to.z - viewBlock.z - 0.5) > 0.9f &&
                abs(to.z - viewBlock.z) <= 1.0f) {
                selectedFace = Block::Back;
            } else if (abs(to.z - viewBlock.z - 0.5) < 0.1f) {
                selectedFace = Block::Front;
            }

            if (abs(to.y - viewBlock.y - 0.5) > 0.9f &&
                abs(to.y - viewBlock.y) <= 1.0f) {
                selectedFace = Block::Bottom;
            } else if (abs(to.y - viewBlock.y - 0.5) < 0.1f) {
                selectedFace = Block::Top;
            }
        }
    }
}
