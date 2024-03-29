#pragma once

#include "renderer.h"


////////////Important values when it comes to bitwise operations//
#define xBits 0b1111
#define xDivision 1

#define yBits 0b11111111111100000000
#define yDivision 256

#define zBits 0b11110000
#define zDivision 16

#define blockTypeBits 0b11110000000000000000000
#define blockTypeDivision 1048576
//////////////////////////////////////////////////////////////////


// TODO: Modify visible chunks and adjust what is being rendered
// based on camera/player position.
class World {
public:
    World(unsigned int size);

    ~World();

    // Draws out every chunk in visibleChunks
    void draw(glm::mat4 view);

private:
    class Chunk;

    void shaderInit();

    Renderer renderer{};
    std::vector<std::vector<Chunk>> visibleChunks;
    // std::vector< std::vector<Chunk> > visibleChunks;

    // Unsure if this is a good idea.
    // std::unordered_map<Chunk, int> visibleChunks;
};

class World::Chunk {
public:
    // Generates buffers and VAO, creates terrain for chunk.
    Chunk(int x, int y, int z);

    ~Chunk();

    bool findBlock(unsigned int thisBlock) const;

    std::pair<std::vector<unsigned int>, std::vector<float>> generateMesh();

private:
    // Helper functions for the constructor so it's easier to read.
    void generateTerrain();

    // NOTE: These following functions check if a side of a block
    // is obscured, and only work due to the way the world is
    // generated. Whenever blocks outside of normal worldgen
    // are to be added into the world, they will need to be sorted
    // in the same way as these.
    //
    // Sort method:
    // Blocks above another in the same xz column should be sorted from
    // bottom to top, i.e find the first occurence of the xz coordinate
    // in the blockArray and then place it right after the last occurence.
    //
    // NOTE: Problem with this: Chunk borders are always assumed to be obscured,
    // implement some check/fix for this after at least a second chunk has been
    // made, this will need to happen in chunk, but the function check will
    // likely need to happen in this class and used by the world class.
    bool obsBack(unsigned int thisBlock);
    bool obsFront(unsigned int thisBlock);
    bool obsTop(unsigned int thisBlock);
    bool obsBottom(unsigned int thisBlock);
    bool obsLeft(unsigned int thisBlock);
    bool obsRight(unsigned int thisBlock);

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
    //
    std::vector<unsigned int> blockArray;

    int xPos;
    int yPos;
    int zPos;
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
