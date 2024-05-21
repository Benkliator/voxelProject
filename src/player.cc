#include "player.h"
#include "block.h"
#include "game.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <vector>

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

void Player::checkCollisions(glm::vec3& newCameraPos) {
    const std::vector<glm::vec3> checkDirs = {
        { 0.0f, 0.0f, -playerWidth / 2.0f },
        { 0.0f, 0.0f, playerWidth / 2.0f },
        { -playerWidth / 2.0f, 0.0f, 0.0f },
        { playerWidth / 2.0f, 0.0f, 0.0f },
        { playerWidth / 2.0f, 0.0f, playerWidth / 2.0f },
        { -playerWidth / 2.0f, 0.0f, playerWidth / 2.0f },
        { playerWidth / 2.0f, 0.0f, -playerWidth / 2.0f },
        { -playerWidth / 2.0f, 0.0f, -playerWidth / 2.0f },
    };
    // SIDES
    glm::vec3 moveDir = glm::vec3(newCameraPos.x, 0, newCameraPos.z) -
                        glm::vec3(cameraPos.x, 0, cameraPos.z);
    for (float height = 0; height < playerHeight; height += 1.0f) {
        // HACK: Detect if we have two collisions per height and restore the
        //       oldCameraPos.{x,z}
        bool collidedHorizontally = false;
        for (glm::vec3 const& checkDir : checkDirs) {
            float xPos = std::round(newCameraPos.x + checkDir.x);
            float zPos = std::round(newCameraPos.z + checkDir.z);
            ushort checkBlock =
                world->getBlock(xPos, std::round(cameraPos.y) - height, zPos)
                    .value_or(Block::Air << typeOffset);
            if (!isAir(checkBlock)) {
                if (collidedHorizontally) {
                    newCameraPos.x = cameraPos.x;
                    newCameraPos.z = cameraPos.z;
                    break;
                }
                glm::vec3 normal;
                if (checkDir.x != 0.0f &&
                    (std::signbit(checkDir.x) == std::signbit(moveDir.x))) {
                    normal = { 1.0f, 0.0f, 0.0f };
                } else if (checkDir.z != 0.0f && (std::signbit(checkDir.z) ==
                                                  std::signbit(moveDir.z))) {
                    normal = { 0.0f, 0.0f, 1.0f };
                } else {
                    continue;
                }
                newCameraPos -= glm::dot(glm::vec3(moveDir.x, 0, moveDir.z), 
                                glm::vec3(normal.x, 0, normal.z)) * normal;
                collidedHorizontally = true;
            }
        }
    }

    // BELOW
    ushort blockBelow =
        world->getBlock(std::round(newCameraPos.x),
                        std::round(newCameraPos.y - playerHeight),
                        std::round(newCameraPos.z))
        .value_or(Block::Air << typeOffset);
    for (glm::vec3 const& dir : checkDirs) {
        float xPos = std::round(newCameraPos.x + dir.x);
        float zPos = std::round(newCameraPos.z + dir.z);
        ushort nextblockBelow =
            world->getBlock(xPos, std::round(newCameraPos.y - playerHeight), zPos)
                .value_or(Block::Air << typeOffset);
        if (!isAir(nextblockBelow) && !isAir(blockBelow)) {
            if (ySpeed < 0.0f) {
                ySpeed = 0.0f;
                onGround = true;
                float floorPos = std::floor(cameraPos.y - playerHeight) + 0.5f;
                cameraPos.y = floorPos + playerHeight - 1e-5;
                newCameraPos.y = cameraPos.y;
                break;
            }
        } else if (isAir(blockBelow)){
            onGround = false;
            cameraPos.y = newCameraPos.y;
            break;
        }
    }
    // ABOVE
    for (glm::vec3 const& dir : checkDirs) {
        float xPos = std::round(cameraPos.x + dir.x);
        float zPos = std::round(cameraPos.z + dir.z);
        ushort blockAbove = world->getBlock(xPos, std::ceil(cameraPos.y), zPos)
                                .value_or(Block::Air << typeOffset);
        if (!isAir(blockAbove)) {
            if (ySpeed > 0.0f) {
                ySpeed = 0.0f;
                float ceilPos = std::ceil(cameraPos.y) - 0.5f;
                cameraPos.y = std::min(cameraPos.y, ceilPos);
                break;
            }
        }
    }
}

void Player::movePlayer(GLFWwindow* window, float dt) {
    glm::vec3 newCameraPos = cameraPos;
    const float cameraSpeed = 5.5f * dt;
    const float sprintBoost = 1.8f * dt;

    const float gravitySpeed = 23.f * dt;
    const float jumpBoost = 8;

    glm::vec3 resultingSpeed = glm::vec3(0.0, 0.0, 0.0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        resultingSpeed +=
            glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        resultingSpeed -=
            glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        resultingSpeed -= glm::normalize(
            glm::cross(glm::vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        resultingSpeed += glm::normalize(
            glm::cross(glm::vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
    }

    if (resultingSpeed.x != 0 || resultingSpeed.y != 0 ||
        resultingSpeed.z != 0) {
        resultingSpeed = glm::normalize(resultingSpeed);
        newCameraPos += resultingSpeed * cameraSpeed;
    }

    int isSprinting =
        ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) &&
         (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS));
    if (isSprinting) {
        newCameraPos +=
            glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z)) *
            sprintBoost;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (mode == Mode::survival) {
            if (onGround) {
                ySpeed += jumpBoost;
                onGround = false;
            }
        } else {
            newCameraPos.y += cameraSpeed;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (mode == Mode::creative) {
            newCameraPos.y -= cameraSpeed;
        }
    }
    auto modeswitchKey = glfwGetKey(window, GLFW_KEY_G);
    if (modeswitchKey == GLFW_PRESS) {
        if (!modeKeyPressed) {
            modeKeyPressed = false;
            ySpeed = 0.0f;
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

    if (mode == Mode::survival) {
        if (!onGround) {
            ySpeed -= gravitySpeed;
        }
        newCameraPos.y += ySpeed * dt;
        checkCollisions(newCameraPos);
    }
    cameraPos = newCameraPos;

    view();

    unsigned xChunk = (unsigned)cameraPos.x - ((unsigned)(cameraPos.x) % 16);
    unsigned zChunk = (unsigned)cameraPos.z - ((unsigned)(cameraPos.z) % 16);
    glm::uvec3 tempChunkPos = glm::uvec3(xChunk, 0, zChunk);
    if (tempChunkPos != chunkPos) {
        world->reloadChunksAround(xChunk, 0, zChunk);
    }
    chunkPos = tempChunkPos;
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
        if (isAir(tempChunk.getBlock(
                viewBlock.x % 16, viewBlock.y, viewBlock.z % 16))) {
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

    glm::uvec3 roundedPos = glm::uvec3(std::round(cameraPos.x), std::round(cameraPos.y), std::round(cameraPos.z));
    if (placePos == roundedPos || 
        placePos == glm::uvec3(roundedPos.x, roundedPos.y - 1, roundedPos.z)) {
        return;
    }

    tempChunkOpt = world->getChunk(xChunk, 0, zChunk);
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
    hud.renderText(std::string{ "Mode: " } +
                       (mode == Mode::survival ? "survival" : "creative"),
                   5.0f,
                   40.0f,
                   0.5f,
                   glm::vec3{ 1.0f, 1.0f, 1.0f });
    // Crosshair
    hud.renderText("*",
                   SCR_WIDTH / 2.0f,
                   SCR_HEIGHT / 2.0f,
                   0.5f,
                   glm::vec3{ 1.0f, 1.0f, 1.0f });
}

void Player::view() {
    glm::vec3 to = rayCast(0.02f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);
    float it = 0.1f;
    while (it <= 4.0f) {
        auto chunkOpt = world->getChunk(xChunk, 0, zChunk);
        if (chunkOpt.has_value()) {
            Chunk& chunk = chunkOpt.value().get();
            if (!isAir(chunk.getBlock(xBlock % 16, yBlock, zBlock % 16))) {
                break;
            }
        }
        to = rayCast(0.1f + it);

        xBlock = unsigned(std::round(to.x));
        yBlock = unsigned(std::round(to.y));
        zBlock = unsigned(std::round(to.z));

        xChunk = xBlock - (xBlock % 16);
        zChunk = zBlock - (zBlock % 16);
        it += 0.1;
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
