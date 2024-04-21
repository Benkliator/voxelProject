#include "glad/glad.h"

#include "block.h"
#include "chunk.h"
#include "noise.h"
#include "world.h"

#include <array>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Generates buffers and VAO, creates terrain for chunk.
Chunk::Chunk(int x, int z, World* w) : world{ w } {
    // Chunk offset
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    pos = {
        x * 16,
        0,
        z * 16,
    };
    generateTerrain();
}

Chunk::~Chunk() {}

void Chunk::generateMesh() {
    for (size_t i = 0; i < blockArray.size(); i++) {
        unsigned short block = blockArray[i];
        if (isAir(block)) {
            continue;
        }

        unsigned z = i / (worldHeight * 16);
        unsigned y = i % worldHeight;
        unsigned x = (i % (16 * worldHeight)) / worldHeight;
        Block blockType{ (block & typeMask) >> typeOffset };

        // Top
        if (isAir(getBlockGlobal(x, y + 1, z))) {
            blockArray[i] = blockArray[i] | topMask;
            loadFace(&topMeshData, blockType.top, x, y, z);
        }

        // Bottom
        if (isAir(getBlockGlobal(x, y - 1, z))) {
            blockArray[i] = blockArray[i] | bottomMask;
            loadFace(&bottomMeshData, blockType.bottom, x, y, z);
        }

        // Back
        if (isAir(getBlockGlobal(x, y, z - 1))) {
            blockArray[i] = blockArray[i] | backMask;
            loadFace(&backMeshData, blockType.side, x, y, z);
        }

        // Front
        if (isAir(getBlockGlobal(x, y, z + 1))) {
            blockArray[i] = blockArray[i] | frontMask;
            loadFace(&frontMeshData, blockType.side, x, y, z);
        }

        // Left
        if (isAir(getBlockGlobal(x - 1, y, z))) {
            blockArray[i] = blockArray[i] | leftMask;
            loadFace(&leftMeshData, blockType.side, x, y, z);
        }

        // Right
        if (isAir(getBlockGlobal(x + 1, y, z))) {
            blockArray[i] = blockArray[i] | rightMask;
            loadFace(&rightMeshData, blockType.side, x, y, z);
        }
    }
    renderInit();
}

void Chunk::generateMesh(std::vector<unsigned short> blockAreaArray) {
    for (size_t i = 0; i < blockAreaArray.size(); i++) {
        unsigned short block = blockAreaArray[i];
        if (isAir(block)) {
            continue;
        }

        unsigned z = i / (worldHeight * 16);
        unsigned y = i % worldHeight;
        unsigned x = (i % (16 * worldHeight)) / worldHeight;
        Block blockType{ (block & typeMask) >> typeOffset };

        // Top
        if (isAir(getBlockGlobal(x, y + 1, z))) {
            loadFace(&topMeshData, blockType.top, x, y, z);
        }

        // Bottom
        if (isAir(getBlockGlobal(x, y - 1, z))) {
            loadFace(&bottomMeshData, blockType.bottom, x, y, z);
        }

        // Back
        if (isAir(getBlockGlobal(x, y, z - 1))) {
            loadFace(&backMeshData, blockType.side, x, y, z);
        }

        // Front
        if (isAir(getBlockGlobal(x, y, z + 1))) {
            loadFace(&frontMeshData, blockType.side, x, y, z);
        }

        // Left
        if (isAir(getBlockGlobal(x - 1, y, z))) {
            loadFace(&leftMeshData, blockType.side, x, y, z);
        }

        // Right
        if (isAir(getBlockGlobal(x + 1, y, z))) {
            loadFace(&rightMeshData, blockType.side, x, y, z);
        }
    }
    renderInit();
}

void Chunk::reloadMesh(unsigned x, unsigned y, unsigned z) {
    size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
    std::vector<unsigned short> tempBlockArray;
    tempBlockArray.push_back(blockArray[ix]);
    if (y > 0) {
        tempBlockArray.push_back(blockArray[ix - 1]);
    }
    if (y < worldHeight) {
        tempBlockArray.push_back(blockArray[ix + 1]);
    }
    /*  HUGE MASSIVE NOTE:
     *  This will need to update the meshes of adjacent CHUNKS!!!!
     */
    tempBlockArray.push_back(getBlockGlobal(x + 1, y, z));
    tempBlockArray.push_back(getBlockGlobal(x - 1, y, z));
    tempBlockArray.push_back(getBlockGlobal(x, y, z + 1));
    tempBlockArray.push_back(getBlockGlobal(x, y, z - 1));
    /* removeMeshArea(); */
    generateMesh(tempBlockArray);
}

void Chunk::draw(unsigned shader) {
    glBindVertexArray(VAO);
    glUniform3uiv(glGetUniformLocation(shader, "chunkPos"),
                 1,
                 glm::value_ptr(pos));
    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
}

unsigned short Chunk::getBlock(unsigned x, unsigned y, unsigned z) {
    size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
    return blockArray[ix];
}

unsigned short Chunk::getBlockGlobal(long dX, long dY, long dZ) {
    if (dY < 0 || dY >= worldHeight) {
        return errorBlock;
    }
    if (dX >= 0 && dX <= 15 && dZ >= 0 && dZ <= 15) {
        // Can use local version
        return getBlock(dX, dY, dZ);
    } else {
        // Find the block in all chunks
        return world->getBlock(pos.x + dX, pos.y + dY, pos.z + dZ);
    }
}

bool Chunk::removeBlock(unsigned x, unsigned y, unsigned z) {
    if (!isAir(getBlock(x, y, z))) {
        size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
        blockArray[ix] = 0;
        clearMesh();
        generateMesh();
        // reloadMesh(x, y, z);
        return true;
    };
    return false;
}

bool Chunk::removeBlockMesh(unsigned x, unsigned y, unsigned z) {
    // TODO: Implement, for implement:
    // How can we know where a blocks mesh data 
    // is in the vertex and index mesh?
    size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
    unsigned block = blockArray[ix];
    if (isAir(block)) {
        return false;
    }
    return true;
}

void Chunk::clearMesh()  {
    vertexMesh.clear();
    indexMesh.clear();
}

glm::uvec3 Chunk::getPos() {
    return pos;
}

void Chunk::generateTerrain() {
    float freq = 0.8396323343; // Frequency
    float amp = 1.35;        // Amplifier
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            // Uses perlin function to calculate height for xz position
            float noise =
                perlin(((z + pos.x) * freq) / 16, ((x + pos.z) * freq) / 16);
            float height = noise * amp + 1;
            for (unsigned y = 0; y < worldHeight; y++) {
                // Using the following line in an if or switch case
                // can let you dynamically decide block generations at
                // different (x, y, z) values.
                enum Block::BlockType bt;
                if (y < 3) {
                    bt = Block::Water;
                } else if (y > height && y < height + 1) {
                    bt = Block::Grass;
                } else if (y < height) {
                    bt = Block::Dirt;
                } else {
                    bt = Block::Air;
                }
                unsigned val = bt << typeOffset;
                blockArray.push_back(val);
            }
        }
    }
}

void Chunk::renderInit() {
    indexSize = indexMesh.size();
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexMesh.size() * sizeof(GLuint),
                 vertexMesh.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexMesh.size() * sizeof(unsigned),
                 indexMesh.data(),
                 GL_STATIC_DRAW);

    glVertexAttribIPointer(
        0, 2, GL_UNSIGNED_INT, 2 * sizeof(GLuint), (void*)(0));
    glEnableVertexAttribArray(0);
}

void Chunk::loadFace(const MeshData* data, unsigned bt, unsigned x, unsigned y, unsigned z) {
    indexMesh.push_back((vertexMesh.size() / 2) + 0);
    indexMesh.push_back((vertexMesh.size() / 2) + 3);
    indexMesh.push_back((vertexMesh.size() / 2) + 1);
    indexMesh.push_back((vertexMesh.size() / 2) + 2);
    indexMesh.push_back((vertexMesh.size() / 2) + 1);
    indexMesh.push_back((vertexMesh.size() / 2) + 3);
    std::array<unsigned short, 4> occlusionArray = getOcclusion(x, y, z, data->blockFace);
    size_t j = 0;
    size_t t = 0;
    for (size_t i = 0; i < 4; i++) {
        GLuint xi = x + data->faceCoords[j++];
        GLuint yi = y + data->faceCoords[j++];
        GLuint zi = z + data->faceCoords[j++];
        GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18;

        GLuint texX = data->texCoords[t++] + ((bt & zMask) >> zOffset);
        GLuint texY = data->texCoords[t++] + bt & xMask;
        GLuint texData = texX | texY << 8;

        vertexMesh.push_back(vertex);
        vertexMesh.push_back(texData);
    }
}

std::array<unsigned short, 4>
Chunk::getOcclusion(unsigned x, unsigned y, unsigned z, unsigned short face) {
    // This is really ugly lol, will probably rework at some point.
    std::array<unsigned short, 4> vertexOcclusion{ 0, 0, 0, 0 };
    switch (face) {
    case Block::Top: {
        if (!isAir(getBlockGlobal(x, y + 1, z + 1))) {
            vertexOcclusion[1] += 1;
            vertexOcclusion[2] += 1;
        }
        if (!isAir(getBlockGlobal(x, y + 1, z - 1))) {
            vertexOcclusion[0] += 1;
            vertexOcclusion[3] += 1;
        }
        if (!isAir(getBlockGlobal(x + 1, y + 1, z))) {
            vertexOcclusion[0] += 1;
            vertexOcclusion[1] += 1;
        }
        if (!isAir(getBlockGlobal(x - 1, y + 1, z))) {
            vertexOcclusion[2] += 1;
            vertexOcclusion[3] += 1;
        }
        if (!isAir(getBlockGlobal(x + 1, y + 1, z + 1))) {
            vertexOcclusion[1] += 1;
        }
        if (!isAir(getBlockGlobal(x - 1, y + 1, z + 1))) {
            vertexOcclusion[2] += 1;
        }
        if (!isAir(getBlockGlobal(x + 1, y + 1, z - 1))) {
            vertexOcclusion[0] += 1;
        }
        if (!isAir(getBlockGlobal(x - 1, y + 1, z - 1))) {
            vertexOcclusion[3] += 1;
        }
    } break;
    case Block::Back: {
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[0] = 1;
            vertexOcclusion[3] = 1;
        }
        if (!isAir(getBlock(x, y + 1, z))) {
            vertexOcclusion[1] = 1;
            vertexOcclusion[2] = 1;
        }
        if (!isAir(getBlockGlobal(x, y - 1, z - 1))) {
            vertexOcclusion[0] = 3;
            vertexOcclusion[3] = 3;
        }
    } break;
    case Block::Front: {
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[1] = 1;
            vertexOcclusion[2] = 1;
        }
        if (!isAir(getBlock(x, y + 1, z))) {
            vertexOcclusion[0] = 1;
            vertexOcclusion[3] = 1;
        }
        if (!isAir(getBlockGlobal(x, y - 1, z + 1))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Right: {
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[1] = 1;
            vertexOcclusion[2] = 1;
        }
        if (!isAir(getBlock(x, y + 1, z))) {
            vertexOcclusion[0] = 1;
            vertexOcclusion[3] = 1;
        }
        if (!isAir(getBlockGlobal(x + 1, y - 1, z))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Left: {
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[1] = 1;
            vertexOcclusion[2] = 1;
        }
        if (!isAir(getBlock(x, y + 1, z))) {
            vertexOcclusion[0] = 1;
            vertexOcclusion[3] = 1;
        }
        if (!isAir(getBlockGlobal(x - 1, y - 1, z))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Bottom: {
            vertexOcclusion[0] = 3;
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
            vertexOcclusion[3] = 3;
    }
    }
    return vertexOcclusion;
}
