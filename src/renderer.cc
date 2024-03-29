#include "glad/glad.h"

#include "renderer.h"
#include "utility.h"

#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    textureMap = loadTexture("./res/textures/Blockmap2.png", GL_RGBA);
    shaderInit();
    renderInit();
}

Renderer::Renderer(std::vector<float> vertices,
                   std::vector<unsigned int> indices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    textureMap = loadTexture("./res/textures/Blockmap2.png", GL_RGBA);
    vertexMesh = vertices;
    indexMesh = indices;

    shaderInit();
    renderInit();
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteShader(shaderProgram);
    std::vector<float>().swap(vertexMesh);
    std::vector<unsigned int>().swap(indexMesh);
}

void Renderer::changeRendering(std::vector<float> newVertices,
                               std::vector<unsigned int> newIndices) {
    vertexMesh = newVertices;
    indexMesh = newIndices;
    renderInit();
}

// TODO: Implement insertion-sort for this.
void Renderer::addRendering(std::vector<float> addVertices,
                            std::vector<unsigned int> addIndices) {
    for (size_t i = 0; i < addIndices.size(); i += 6) {
        int size = vertexMesh.size() / 9;
        indexMesh.push_back(addIndices[i] + size);
        indexMesh.push_back(addIndices[i + 1] + size);
        indexMesh.push_back(addIndices[i + 2] + size);
        indexMesh.push_back(addIndices[i + 3] + size);
        indexMesh.push_back(addIndices[i + 4] + size);
        indexMesh.push_back(addIndices[i + 5] + size);
    }
    for (auto&& it : addVertices) {
        vertexMesh.push_back(it);
    }
}

void Renderer::render(glm::mat4 view) {
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, textureMap);

    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 1.8f, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                       1,
                       GL_FALSE,
                       &projection[0][0]);
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

    // glm::mat4 model = glm::mat4(1.0f);
    // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1,
    // GL_FALSE, &model[0][0]);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, indexMesh.size(), GL_UNSIGNED_INT, 0);
}

void Renderer::renderInit() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexMesh.size() * sizeof(float),
                 vertexMesh.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexMesh.size() * sizeof(unsigned int),
                 indexMesh.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
}

void Renderer::shaderInit() {
    unsigned int vertexShader =
        loadShader(GL_VERTEX_SHADER, "./res/shaders/main.vert");
    unsigned int fragmentShader =
        loadShader(GL_FRAGMENT_SHADER, "./res/shaders/main.frag");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
