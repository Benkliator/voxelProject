#pragma once

#include <glm/fwd.hpp>
#include <vector>

class Skybox {
public:
    Skybox(std::vector<const char*> texturePath);

    ~Skybox();

    // Does what you think it does.
    void draw(glm::mat4 view, float time);

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int shaderProgram;
    unsigned int texture;
};
