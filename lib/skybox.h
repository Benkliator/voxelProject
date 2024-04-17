#pragma once

#include <glm/fwd.hpp>
#include <vector>

class Skybox {
public:
    Skybox();

    ~Skybox();

    // Does what you think it does.
    void draw(glm::mat4& view, float time);

private:
    unsigned VAO;
    unsigned VBO;
    unsigned EBO;

    unsigned shaderProgram;

    unsigned dayTexture;
    unsigned nightTexture;
    unsigned duskTexture;
};
