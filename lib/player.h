#pragma once

#include <glm/fwd.hpp>

#include "hud.h"
#include "input.h"
#include "world.h"

class Player : protected Camera {
public:
    Player(World* w, glm::vec3);
    ~Player();

    void moveMouse(GLFWwindow*, float, float);
    void movePlayer(GLFWwindow*, float);
    void selectBlock(GLFWwindow*, float);
    void breakBlock();
    void placeBlock();
    void draw(std::string&);

    glm::mat4 worldLook();
    glm::mat4 skyLook();

    void checkChunk();
private:
    void view();
    void checkCollisions(glm::vec3&);

    World* world;
    Hud hud{};

    Camera skyCam;

    glm::uvec3 viewBlock;
    Block::BlockFace viewFace;
    float ySpeed = 0.0f;
    bool onGround = false;
    glm::uvec3 chunkPos;

    Block::BlockType selectedBlock = Block::Dirt;
    Block::BlockFace selectedFace = Block::Top;

    enum Mode : ushort {
        survival,
        creative,
    };
    enum Mode mode = survival;
    bool modeKeyPressed = false;

    const float playerHeight = 1.5f;
    // NOTE: (playerWidth / 2.0f) out to all four sides
    const float playerWidth = 0.6f;
};
