#pragma once

#include <glm/fwd.hpp>
#include <vector>

class Renderer {
public:
    Renderer();
    Renderer(std::vector<float> vertices, std::vector<unsigned int> indices);

    ~Renderer();

    void changeRendering(std::vector<float> newVertices,
                         std::vector<unsigned int> newIndices);

    void addRendering(std::vector<float> addVertices,
                      std::vector<unsigned int> addIndices);

    void render(glm::mat4 view);
    void renderInit();

private:
    void shaderInit();

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int shaderProgram;

    std::vector<float> vertexMesh;
    std::vector<unsigned int> indexMesh;

    unsigned int textureMap;
};
