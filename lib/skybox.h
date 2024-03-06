#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>

#include "../lib/BensGLUtilities.h"

class Skybox{
    public:
        Skybox(std::vector<const char*> texturePath);

        // Does what you think it does.
        void draw(glm::mat4 view);
    private:
        unsigned int vertexArrayObjID;
        unsigned int vertexBufferObjID;
        unsigned int elementBufferObjID;

        unsigned int shaderProgram;        
        unsigned int texture;
};
