#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "input.h"
#include "skybox.h"
#include "world.h"
#include "hud.h"

const unsigned int SCR_WIDTH = 1620;
const unsigned int SCR_HEIGHT = 900;

#define RENDER_DISTANCE 50

class Game {
public:
    Game();
    ~Game();

    void gameLoop();

    void mouseCallback(double xposIn, double yposIn);

private:
    void processInput();

    Hud* hud = nullptr;;

    Camera* playerCam = nullptr;
    Camera* skyboxCam = nullptr;

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
