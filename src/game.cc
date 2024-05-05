#include "glad/glad.h"

#include "game.h"
#include "world.h"

#include <iostream>

Game::Game() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Game", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create window" << std::endl;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSetWindowUserPointer(window, this);
    auto mouseMotionCallback = [](GLFWwindow* w, double x, double y) {
        static_cast<Game*>(glfwGetWindowUserPointer(w))
            ->mouseMotionCallback(x, y);
    };
    glfwSetCursorPosCallback(window, mouseMotionCallback);

    glfwSetWindowUserPointer(window, this);
    auto mouseScrollCallback = [](GLFWwindow* w, double x, double y) {
        static_cast<Game*>(glfwGetWindowUserPointer(w))
            ->mouseScrollCallback(x, y);
    };
    glfwSetScrollCallback(window, mouseScrollCallback);

    auto mouseClickCallback =
        [](GLFWwindow* w, int button, int action, int mods) {
            static_cast<Game*>(glfwGetWindowUserPointer(w))
                ->mouseClickCallback(button, action, mods);
        };
    glfwSetMouseButtonCallback(window, mouseClickCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
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

    skybox = new Skybox{};
    world = new World{ RENDER_DISTANCE };
    player = new Player{ world };
}

Game::~Game() {
    delete player;
    delete world;
    delete skybox;
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
        glm::mat4 view = player->worldLook();
        world->draw(view);

        view = player->skyLook();
        skybox->draw(view, currentFrame);
        player->draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Game::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    player->movePlayer(window, deltaTime);
}

void Game::mouseMotionCallback(double xposIn, double yposIn) {
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

    player->moveMouse(window, xoffset, yoffset);
}

void Game::mouseScrollCallback(double x, double y) {
    player->selectBlock(window, x, y);
}

void Game::mouseClickCallback(int button, int action, int mods) {
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        player->breakBlock();
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        player->placeBlock();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    (void)window; // Unused
    glViewport(0, 0, width, height);
}
