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
    void movePlayer(GLFWwindow*);
    void selectBlock(GLFWwindow*, float);
    void breakBlock();
    void placeBlock();
    void draw(std::string&);

    glm::mat4 worldLook();
    glm::mat4 skyLook();

    void checkChunk();
    void timeStep(float);
private:
    void view();
    void checkCollision();

    void checkCollisionX(int);
    void checkCollisionZ(int);
    void yCollision();

    World* world;
    Hud hud{};

    Camera skyCam;

    glm::vec3 nextCameraPos;
    glm::vec3 prevCameraPos;

    glm::uvec3 viewBlock;
    Block::BlockFace viewFace;
    glm::vec3 velocity;
    bool onGround = false;
    bool collision = false;
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
    // NOTE: (playerWidth) out to all four sides
    const float playerWidth = 0.33f;
};
