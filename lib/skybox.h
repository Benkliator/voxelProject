#pragma once

#include <glm/fwd.hpp>

class Skybox {
public:
    Skybox();
    ~Skybox();

    // Does what you think it does.
    void draw(glm::mat4&);

    void update(double);

private:
    unsigned VAO;
    unsigned VBO;
    unsigned EBO;

    unsigned shaderProgram;

    unsigned dayTexture;
    unsigned nightTexture;
    unsigned duskTexture;

    float angle = 0;
};
