#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>

#include "../lib/utility.h"

class Skybox{
    public:
        Skybox(std::vector<const char*> texturePath);

         ~Skybox();

        // Does what you think it does.
        void draw(glm::mat4 view);
    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        unsigned int shaderProgram;
        unsigned int texture;
};
