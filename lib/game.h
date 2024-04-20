#pragma once

#include "hud.h"
#include "player.h"
#include "skybox.h"
#include "world.h"

const unsigned SCR_WIDTH = 1620;
const unsigned SCR_HEIGHT = 900;

const unsigned RENDER_DISTANCE = 40;

class Game {
public:
    Game();
    ~Game();

    void gameLoop();
    void mouseMotionCallback(double, double);
    void mouseClickCallback(int, int, int);

private:
    void processInput();

    Player* player = nullptr;
    Hud* hud = nullptr;
    Skybox* skybox = nullptr;
    World* world = nullptr;

    GLFWwindow* window = nullptr;

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};

void framebufferSizeCallback(GLFWwindow*, int, int);
