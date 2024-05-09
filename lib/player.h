#pragma once

#include <glm/fwd.hpp>

#include "hud.h"
#include "input.h"
#include "world.h"

class Player : protected Camera {
public:
    Player(World* w);
    ~Player();

    void moveMouse(GLFWwindow*, float, float);
    void movePlayer(GLFWwindow*, float);
    void selectBlock(GLFWwindow*, float, float);
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

    Block::BlockType selectedBlock = Block::Dirt;

    const float playerHeight = 1.30f;
};
