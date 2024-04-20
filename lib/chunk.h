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
 * 0000 0000   0000 0000
 *   block
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
    void draw(unsigned);

    unsigned short getBlock(unsigned, unsigned, unsigned);
    unsigned short getBlockGlobal(long, long, long);
    glm::uvec3 getPos();

private:
    void generateTerrain();
    void renderInit(std::vector<GLuint>&, std::vector<unsigned>&);

    std::array<unsigned short, 4>
    getOcclusion(unsigned, unsigned, unsigned, unsigned short);
    std::vector<unsigned short> blockArray;
    glm::uvec3 pos;

    World* world;

    unsigned VAO;
    unsigned VBO;
    unsigned EBO;
    unsigned indexSize;
};

//  x  y  z
const unsigned topFace[] {
    1, 1, 0,
    1, 1, 1,
    0, 1, 1,
    0, 1, 0,
};

const unsigned backFace[] {
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,
    0, 0, 0,
};

const unsigned frontFace[] {
    1, 1, 1,
    1, 0, 1,
    0, 0, 1,
    0, 1, 1,
};

const unsigned leftFace[] {
    0, 1, 1,
    0, 0, 1,
    0, 0, 0,
    0, 1, 0,
};

const unsigned rightFace[] {
    1, 1, 0,
    1, 0, 0,
    1, 0, 1,
    1, 1, 1,
};

const unsigned bottomFace[] {
    0, 0, 0,
    0, 0, 1,
    1, 0, 1,
    1, 0, 0,
};

const unsigned topTexcoord[] {
    1, 0,
    1, 1,
    0, 1,
    0, 0,
};

const unsigned backTexcoord[] {
    0, 1,
    0, 0,
    1, 0,
    1, 1,
};

const unsigned frontTexcoord[] {
    0, 0,
    0, 1,
    1, 1,
    1, 0,
};

const unsigned leftTexcoord[] {
    1, 0,
    1, 1,
    0, 1,
    0, 0,
};

const unsigned rightTexcoord[] {
    0, 0,
    0, 1,
    1, 1,
    1, 0,
};

const unsigned bottomTexcoord[] {
    0, 0,
    0, 1,
    1, 1,
    1, 0,
};
