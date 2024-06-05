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
    unsigned offset = 2000;
    glm::vec3 startCoords{ 16 * offset + RENDER_DISTANCE * 8,
                           70,
                           16 * offset + RENDER_DISTANCE * 8 };
    world = new World{ RENDER_DISTANCE, startCoords };
    player = new Player{ world, startCoords };
}

Game::~Game() {
    delete player;
    delete world;
    delete skybox;
    glfwTerminate();
}

void Game::gameLoop() {
    float currentFrame = 0.0f;
    glfwSetTime(0);

    std::jthread tickThread(&Game::tickUpdate, this);

    while (!glfwWindowShouldClose(window)) {
        {
            currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            std::lock_guard<std::mutex> lock(gameMutex);

            world->meshCatchup();

            glm::mat4 view = player->worldLook();
            world->draw(view);

            view = player->skyLook();
            skybox->draw(view);

            std::string fps = std::to_string(static_cast<int>(1 / deltaTime));
            player->draw(fps);

            player->timeStep(deltaTime * static_cast<float>(tickRate));
            player->checkChunk();
        }
        gameCV.notify_one();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Game::tickUpdate() {
    while (!glfwWindowShouldClose(window)) {
        const auto start = std::chrono::high_resolution_clock::now();
        {
            ++currentTick;
            std::unique_lock<std::mutex> lock(gameMutex);

            processInput();
            skybox->update(currentTick / static_cast<double>(tickRate));

            //gameCV.wait(lock);
            //gameCV.notify_one();
        }
        const auto end = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds((1000 / tickRate)) -
                                    std::chrono::duration_cast<std::chrono::milliseconds>((end - start)));
    }
}

void Game::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    player->movePlayer(window);
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

    std::unique_lock<std::mutex> lock(gameMutex);
    player->moveMouse(window, xoffset, yoffset);
}

void Game::mouseScrollCallback(double, double y) {
    player->selectBlock(window, y);
}

void Game::mouseClickCallback(int button, int action, int mods) {
    (void)mods;
    std::unique_lock<std::mutex> lock(gameMutex);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        player->breakBlock();
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        player->placeBlock();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    (void)window; // Unused
    glViewport(0, 0, width, height);
}
