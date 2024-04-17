#include "skybox.h"


Skybox::Skybox() {
    float vertices[]{ -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f, 0.5f,
                      -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
                      -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f, 0.5f };

    unsigned indices[]{
        0, 2, 1,          // Front
        0, 3, 2, 4, 5, 6, // Back
        4, 6, 7, 0, 4, 7, // Left
        0, 7, 3, 1, 6, 5, // Right
        1, 2, 6, 3, 6, 2, // Top
        3, 7, 6, 0, 1, 5, // Bottom
        0, 5, 4,
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
        "./res/textures/daySky.png", "./res/textures/daySky.png",
        "./res/textures/daySky.png", "./res/textures/daySky.png",
        "./res/textures/daySkyMoon.png", "./res/textures/daySky.png",
    };
    dayTexture = loadTextureCube(skyDay, GL_RGB);

    std::vector<const char*> skyDusk{
        "./res/textures/duskSky.png", "./res/textures/duskSky.png",
        "./res/textures/duskSky.png", "./res/textures/duskSky.png",
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
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(unsigned), (void*)(0));
    glEnableVertexAttribArray(0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteShader(shaderProgram);
}

void Skybox::draw(glm::mat4& view, float time) {
    glUseProgram(shaderProgram);
    float angle = glm::radians(time * 2);
    if (sin(angle) < -0.1) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
    } else if (0.1 < sin(angle)) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexture);
    } else {
        glBindTexture(GL_TEXTURE_CUBE_MAP, duskTexture);
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
                       &projection[0][0]);
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scaling"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(scaling));

    glm::mat4 model{1.0};
    model = glm::rotate(model, angle, glm::vec3(1, 0, 0)); 
    // where x, y, z is axis of rotation
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(model));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glCullFace(GL_BACK);
    glDepthMask(true);
}
