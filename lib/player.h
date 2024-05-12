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
    void draw();

    glm::mat4 worldLook();
    glm::mat4 skyLook();

private:
    void view();
    bool checkHitbox();

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

    const float playerHeight = 1.8f;
    // NOTE: 0.3 out to all four sides
    const float playerWidth = 0.6f;
};
