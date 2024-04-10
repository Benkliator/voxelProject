#include "game.h"

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
    auto mouseCallback = [](GLFWwindow* w, double x, double y)
    {
        static_cast<Game*>(glfwGetWindowUserPointer(w))->mouseCallback(x, y);
    };
    glfwSetCursorPosCallback(window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    // NOTE: DOES NOT WORK ON VMS
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }

    std::vector<const char*> skyFaces{
        "./res/textures/monotoneSky.png", "./res/textures/monotoneSky.png",
        "./res/textures/monotoneSky.png", "./res/textures/monotoneSky.png",
        "./res/textures/monotoneSky.png", "./res/textures/monotoneSky.png",
    };
    skybox = new Skybox{skyFaces};
    world  = new World {20};

    playerCam = new Camera{};
    skyboxCam = new Camera{};
}

Game::~Game() {
    glfwTerminate();
    delete world;
    delete skybox;
    delete playerCam;
    delete skyboxCam;
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
        skybox->draw(view);

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
