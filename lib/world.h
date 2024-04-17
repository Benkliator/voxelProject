#pragma once
#include "glad/glad.h"

#include <array>
#include <vector>
#include <iostream>
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "block.h"
#include "perlin.h"
#include "utility.h"

////////////Important values when it comes to bitwise operations//
const long xBits =              0b00000000000000000001111;
const long yBits =              0b00000001111111100000000;
const long zBits =              0b00000000000000011110000;
const long blockTypeBits =      0b11110000000000000000000;

const long xBitOffset =         __builtin_ctzl(xBits);
const long yBitOffset =         __builtin_ctzl(yBits);
const long zBitOffset =         __builtin_ctzl(zBits);
const long blockTypeBitOffset = __builtin_ctzl(blockTypeBits);

const long worldHeight =        256;
//////////////////////////////////////////////////////////////////

struct chunkPos {
    float x;
    float y;
    float z;
};

// TODO: Modify visible chunks and adjust what is being rendered
// based on camera/player position.
class World {
public:
    World(unsigned size);
    ~World();

    // Draws out every chunk in visibleChunks
    void draw(glm::mat4& view);

    // TODO: impelent?
    bool findChunk(chunkPos pos);
private:
    class Chunk;
    void shaderInit();

    std::vector<Chunk> visibleChunks;
    unsigned textureMap;
    unsigned shaderProgram;
    glm::mat4 projection;
};

class World::Chunk {
public:
    // Generates buffers and VAO, creates terrain for chunk.
    Chunk(int x, int y, int z);
    ~Chunk();

    void generateMesh();
    void draw();

    unsigned getBlock(unsigned x, unsigned y, unsigned z);
    chunkPos getPos();
private:
    // Helper functions for the constructor so it's easier to read.
    void generateTerrain();
    void renderInit(std::vector<float> vertexMesh, std::vector<unsigned>indexMesh);

    std::array<float, 4> getOcclusion(unsigned x, unsigned y, unsigned z, unsigned short face);
    // Bit values for each element in the blockArray vector:
    //
    // 0000   0000 0000   0000 0000 0000   0000 0000
    // ????     block          ypos        zpos xpos
    //
    // xpos: X position for block in chunk, first 4 bits (0 - 15)
    // zpos: Z position for block in chunk, second 4 bits (0 - 15)
    // ypos: Y position for block in chunk, third 12 bits (0 - 4096)
    //
    // block: Describes which blocktype this is, fourth 8 bits (0 - 256)
    //
    // ????: Will be used for additional block data, ex. transparency. To be
    // added.
    std::vector<unsigned> blockArray;
    chunkPos pos;

    unsigned VAO;
    unsigned VBO;
    unsigned EBO;
    unsigned indexSize;
};

// NOTE: Might want to put these in a proper data structure.
inline const float backVertices[]{
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // top-right
    0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // bottom-right
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // bottom-left
    -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f  // top-left
};
inline const float frontVertices[]{
    0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top-right
    0.5f,  -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top-left
};
inline const float leftVertices[]{
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // top-right
    -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // top-left
};
inline const float rightVertices[]{
    0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top-right
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // bottom-right
    0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top-left
};
inline const float bottomVertices[]{
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, // top-right
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-right
    0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // bottom-left
    0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-left
};
inline const float topVertices[]{
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
    0.5f,  0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    -0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
};
