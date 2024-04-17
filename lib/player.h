#pragma once

#include "input.h"
#include "position.h"

class Player {
public:
    Player();
    ~Player();

    void moveMouse(GLFWwindow* window, float xi, float yi);
    void movePlayer(GLFWwindow* window, float dt);

    glm::mat4 worldLook();
    glm::mat4 skyLook();
private:
    void view();

    Camera* worldCam = nullptr;
    Camera* skyCam = nullptr;

    floatPos pos;
};
