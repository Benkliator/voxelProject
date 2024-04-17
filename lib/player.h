#pragma once

#include "input.h"

class Player {
public:
    Player();
    ~Player();

    void moveMouse(GLFWwindow* window, float xi, float yi);
    void movePlayer(GLFWwindow* window, float dt);
    void breakBlock();
    void placeBlock();

    glm::mat4 worldLook();
    glm::mat4 skyLook();

private:
    void view();

    Camera* worldCam = nullptr;
    Camera* skyCam = nullptr;

    glm::vec3 pos;
};
