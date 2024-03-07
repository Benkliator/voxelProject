#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <utility>
#include <cmath>

#include "../lib/BensGLUtilities.h"
#include "../lib/perlin.h"
#include "../lib/block.h"

class Chunk
{
    public:
        // Generates buffers and VAO, creates terrain for chunk.
        Chunk(glm::vec3 offset);

        ~Chunk();

        // Check if blocks are obscured by the other chunk
        // TODO: Implement.
        void cornerCheck(std::vector<Chunk>& otherChunk);

        bool findBlock(glm::vec3 blockPos);

        glm::vec3 returnPos();

        std::pair< std::vector<unsigned int>, std::vector<float> > generateMesh();
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
        bool obsBack      (unsigned int thisBlock);
        bool obsFront     (unsigned int thisBlock);
        bool obsTop       (unsigned int thisBlock);
        bool obsBottom    (unsigned int thisBlock);
        bool obsLeft      (unsigned int thisBlock);
        bool obsRight     (unsigned int thisBlock);

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
        // ????: Will be used for additional block data, ex. transparency. To be added.
        //
        std::vector<unsigned long> blockArray;

        glm::vec3 chunkPos;

};

// TODO: Add normals and modify the contents
// of the functions using them accordingly.
//
// TODO: Make texcoord arrays and finish implementing them in the createMesh func.
//
// NOTE: Might want to put these in a proper data structure.
inline float blockVertices[]{
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,// top-right
        0.5f, -0.5f,  0.5f, 1.0f, 1.0f,// bottom-right
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,// top-left

        0.5f,  0.5f, -0.5f, 1.0f, 0.0f,// top-right
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,// bottom-right
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f,// top-left
};
