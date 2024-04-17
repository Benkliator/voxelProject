#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <iostream>

#include "skybox.h"
#include "world.h"
#include "hud.h"
#include "player.h"

const unsigned SCR_WIDTH  = 1620;
const unsigned SCR_HEIGHT = 900;

#define RENDER_DISTANCE 10

class Game {
public:
    Game();
    ~Game();

    void gameLoop();

    void mouseMotionCallback(double xposIn, double yposIn);

    void mouseClickCallback(int button, int action, int mods);

private:
    void processInput();

    Player* player = nullptr;

    Hud* hud = nullptr;;

    Skybox* skybox = nullptr;
    World* world = nullptr;

    GLFWwindow* window = nullptr;

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
