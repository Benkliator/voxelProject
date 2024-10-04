#pragma once

#include "gametime.h"
#include <glm/fwd.hpp>

class Skybox {
public:
    Skybox(GameTime*);
    ~Skybox();

    // Does what you think it does.
    void draw(glm::mat4&);

    void update();

private:
    unsigned VAO;
    unsigned VBO;
    unsigned EBO;

    unsigned shaderProgram;

    unsigned dayTexture;
    unsigned nightTexture;
    unsigned duskTexture;

    GameTime* gameTime = nullptr;
    GameTime::TimeState timeState;
    float angle;
};
