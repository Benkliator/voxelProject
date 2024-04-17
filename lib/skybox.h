#pragma once

#include "glad/glad.h"
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "utility.h"

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
