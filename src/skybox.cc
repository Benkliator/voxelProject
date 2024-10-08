#include "glad/glad.h"

#include "skybox.h"
#include "utility.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

Skybox::Skybox(GameTime* t) : gameTime { t } {
    float vertices[]{
        -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f, 0.5f,
    };

    unsigned indices[]{
        0, 2, 1, 0, 3, 2, // Front
        4, 5, 6, 4, 6, 7, // Back
        0, 4, 7, 0, 7, 3, // Left
        1, 6, 5, 1, 2, 6, // Right
        3, 6, 2, 3, 7, 6, // Top
        0, 1, 5, 0, 5, 4, // Bottom
    };
    unsigned vertexShader =
        loadShader(GL_VERTEX_SHADER, "./res/shaders/skybox.vert");
    unsigned fragmentShader =
        loadShader(GL_FRAGMENT_SHADER, "./res/shaders/skybox.frag");
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    std::vector<const char*> skyNight{
        "./res/textures/monotoneSky.png", "./res/textures/monotoneSky.png",
        "./res/textures/monotoneSky.png", "./res/textures/monotoneSky.png",
        "./res/textures/monotoneSky.png", "./res/textures/monotoneSkyMoon.png",
    };
    nightTexture = loadTextureCube(skyNight, GL_RGB);

    std::vector<const char*> skyDay{
        "./res/textures/daySky.png",     "./res/textures/daySky.png",
        "./res/textures/daySky.png",     "./res/textures/daySky.png",
        "./res/textures/daySkyMoon.png", "./res/textures/daySky.png",
    };
    dayTexture = loadTextureCube(skyDay, GL_RGB);

    std::vector<const char*> skyDusk{
        "./res/textures/duskSky.png",    "./res/textures/duskSky.png",
        "./res/textures/duskSky.png",    "./res/textures/duskSky.png",
        "./res/textures/duskSkySun.png", "./res/textures/duskSkySun.png",
    };
    duskTexture = loadTextureCube(skyDusk, GL_RGB);

    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Init drawing
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(unsigned), nullptr);
    glEnableVertexAttribArray(0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteShader(shaderProgram);
}

void Skybox::draw(glm::mat4& view) {
    glUseProgram(shaderProgram);
    switch (timeState) {
        case GameTime::Day:
            glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
            break;
        case GameTime::Night:
            glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexture);
            break;
        case GameTime::Dusk:
            glBindTexture(GL_TEXTURE_CUBE_MAP, duskTexture);
            break;
        case GameTime::Dawn:
            glBindTexture(GL_TEXTURE_CUBE_MAP, duskTexture);
            break;
    }
    glBindVertexArray(VAO);

    glDepthMask(false);
    glCullFace(GL_FRONT);
    glm::mat4 scaling = glm::mat4(1.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  1.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  1.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0009f);
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 1.8f, 0.1f, 1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scaling"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(scaling));

    glm::mat4 model{ 1.0 };
    model = glm::rotate(model, angle, glm::vec3(1, 0, 0));
    // where x, y, z is axis of rotation
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(model));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glCullFace(GL_BACK);
    glDepthMask(true);
}

void Skybox::update() {
    angle = gameTime->getSkyboxAngle();
    timeState = gameTime->getTimeState();
}
