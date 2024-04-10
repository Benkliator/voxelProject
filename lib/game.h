#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "input.h"
#include "skybox.h"
#include "world.h"
#include <iostream>
#include <vector>

const unsigned int SCR_WIDTH = 1620;
const unsigned int SCR_HEIGHT = 900;

class Game {
public:
    Game();
    ~Game();

    void gameLoop();

    void mouseCallback(double xposIn, double yposIn);
private:
    void processInput();

    Camera* playerCam{};
    Camera* skyboxCam{};

    Skybox* skybox{};
    World* world{};

    GLFWwindow* window{};

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
