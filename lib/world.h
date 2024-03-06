#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "../lib/chunk.h"
#include "../lib/renderer.h"

// TODO: Modify visible chunks and adjust what is being rendered
// based on camera/player position.
class World
{
    public:
        World(unsigned int size);

        ~World();

        // Draws out every chunk in visibleChunks
        void draw(glm::mat4 view);
    private:
        void shaderInit();

        Renderer renderer{};
        std::vector<Chunk> visibleChunks;
};
