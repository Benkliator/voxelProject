#include "game.h"
#include <GLFW/glfw3.h>

#include <glm/fwd.hpp>
#include <iostream>

Game::Game() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Game", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create window" << std::endl;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    auto mouseCallback = [](GLFWwindow* w, double x, double y) {
        static_cast<Game*>(glfwGetWindowUserPointer(w))->mouseCallback(x, y);
    };
    glfwSetCursorPosCallback(window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }

    skybox = new Skybox {};
    world  = new World {50};

    playerCam = new Camera{};
    skyboxCam = new Camera{};
    hud = new Hud{};
}

Game::~Game() {
    delete world;
    delete skybox;
    delete playerCam;
    delete skyboxCam;
    delete hud;
    glfwTerminate();
}

void Game::gameLoop() {
    float currentFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = playerCam->lookAt();
        world->draw(view);

        view = skyboxCam->lookAt();
        skybox->draw(view, currentFrame);

        hud->renderText("voxelGame V. FINAL_final.final.mov",
                        5.0f,
                        5.0f,
                        1.0f,
                        glm::vec3{ 0.5, 0.8f, 0.2f });

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Game::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    playerCam->processKeyboardInput(window, deltaTime);
}

void Game::mouseCallback(double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    playerCam->processMouseMovement(window, xoffset, yoffset);
    skyboxCam->processMouseMovement(window, xoffset, yoffset);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
