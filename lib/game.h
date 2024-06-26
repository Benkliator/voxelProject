#pragma once

#include "player.h"
#include "skybox.h"
#include "world.h"

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

const unsigned SCR_WIDTH = 1620;
const unsigned SCR_HEIGHT = 900;

const unsigned RENDER_DISTANCE = 16;

class Game {
public:
    Game();
    ~Game();

    void gameLoop();
    void mouseMotionCallback(double, double);
    void mouseScrollCallback(double, double);
    void mouseClickCallback(int, int, int);

    void tickUpdate();
private:
    void processInput();

    Player* player = nullptr;
    Skybox* skybox = nullptr;
    World* world = nullptr;

    GLFWwindow* window = nullptr;

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    unsigned tickRate = 20;
    unsigned currentTick = 1;

    std::mutex gameMutex;
    std::condition_variable gameCV;
};

void framebufferSizeCallback(GLFWwindow*, int, int);
