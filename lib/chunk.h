#pragma once

#include "world.h"

#include <array>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>

class World;

/*
 * Bit values for each element in the blockArray vector:
 *
 * 0000   0000 0000   0000 0000 0000   0000 0000
 * ????     block          ypos        zpos xpos
 *
 * xpos: X position for block in chunk, first 4 bits (0 - 15)
 * zpos: Z position for block in chunk, second 4 bits (0 - 15)
 * ypos: Y position for block in chunk, third 12 bits (0 - 4096)
 *
 * block: Describes which blocktype this is, fourth 8 bits (0 - 256)
 *
 * ????: Will be used for additional block data, ex. transparency. To be
 * added.
 */

class Chunk {
public:
    Chunk(int, int, World*);
    ~Chunk();

    void generateMesh();
    void draw();

    unsigned getBlock(unsigned, unsigned, unsigned);
    unsigned getBlockGlobal(long, long, long);
    glm::uvec3 getPos();

private:
    void generateTerrain();
    void renderInit(std::vector<float>&, std::vector<unsigned>&);

    std::array<float, 4>
    getOcclusion(unsigned, unsigned, unsigned, unsigned short);
    std::vector<unsigned> blockArray;
    glm::uvec3 pos;

    World* world;

    unsigned VAO;
    unsigned VBO;
    unsigned EBO;
    unsigned indexSize;
};

// NOTE: Might want to put these in a proper data structure.
const float backVertices[]{
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // top-right
    0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // bottom-right
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // bottom-left
    -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f  // top-left
};

const float frontVertices[]{
    0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top-right
    0.5f,  -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top-left
};

const float leftVertices[]{
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // top-right
    -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // top-left
};

const float rightVertices[]{
    0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top-right
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // bottom-right
    0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top-left
};

const float bottomVertices[]{
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, // top-right
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-right
    0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // bottom-left
    0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-left
};

const float topVertices[]{
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
    0.5f,  0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    -0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
};
