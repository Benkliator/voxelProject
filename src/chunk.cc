#include "glad/glad.h"

#include "block.h"
#include "chunk.h"
#include "perlin.h"
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
    std::vector<GLuint> vertexMesh;
    std::vector<unsigned> indexMesh;

    for (size_t i = 0; i < blockArray.size(); i++) {
        unsigned short block = blockArray[i];
        if (isAir(block)) {
            continue;
        }

        unsigned z = i / (worldHeight * 16);
        unsigned y = i % worldHeight;
        unsigned x = (i % (16 * worldHeight)) / worldHeight;
        Block blockType{ (block & typeMask) >> typeOffset };

        if (isAir(getBlockGlobal(x, y + 1, z))) {
            indexMesh.push_back((vertexMesh.size() / 2) + 0);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 2);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            std::array<unsigned short, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Top);
            size_t j = 0;
            size_t t = 0;
            for (size_t i = 0; i < 4; i++) {
                GLuint xi = x + topFace[j++];
                GLuint yi = y + topFace[j++];
                GLuint zi = z + topFace[j++];

                GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18;
                vertexMesh.push_back(vertex);

                GLuint texX = topTexcoord[t++] + ((blockType.top & zMask) >> zOffset);
                GLuint texY = topTexcoord[t++] + blockType.top & xMask;
                GLuint texcoords = texX | texY << 8;
                vertexMesh.push_back(texcoords);
            }
        }

        if (isAir(getBlockGlobal(x, y, z - 1))) {
            indexMesh.push_back((vertexMesh.size() / 2) + 0);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 2);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            std::array<unsigned short, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Back);
            size_t j = 0;
            size_t t = 0;
            for (size_t i = 0; i < 4; i ++) {
                GLuint xi = x + backFace[j++];
                GLuint yi = y + backFace[j++];
                GLuint zi = z + backFace[j++];

                GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18;
                vertexMesh.push_back(vertex);

                GLuint texX = backTexcoord[t++] + ((blockType.side & zMask) >> zOffset);
                GLuint texY = backTexcoord[t++] + blockType.side & xMask;
                GLuint texcoords = texX | texY << 8;
                vertexMesh.push_back(texcoords);
            }
        }
        if (isAir(getBlockGlobal(x, y, z + 1))) {
            indexMesh.push_back((vertexMesh.size() / 2) + 0);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 2);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            std::array<unsigned short, 4>occlusionArray =
                getOcclusion(x, y, z, Block::Front);
            size_t j = 0;
            size_t t = 0;
            for (size_t i = 0; i < 4; i++) {
                GLuint xi = x + frontFace[j++];
                GLuint yi = y + frontFace[j++];
                GLuint zi = z + frontFace[j++];

                GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18;
                vertexMesh.push_back(vertex);

                GLuint texX = frontTexcoord[t++] + ((blockType.side & zMask) >> zOffset);
                GLuint texY = frontTexcoord[t++] + (blockType.side & xMask);
                GLuint texcoords = texX | texY << 8;
                vertexMesh.push_back(texcoords);
            }
        }

        if (isAir(getBlockGlobal(x - 1, y, z))) {
            indexMesh.push_back((vertexMesh.size() / 2) + 0);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 2);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            std::array<unsigned short, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Left);
            size_t j = 0;
            size_t t = 0;
            for (size_t i = 0; i < 4; i++) {
                GLuint xi = x + leftFace[j++];
                GLuint yi = y + leftFace[j++];
                GLuint zi = z + leftFace[j++];

                GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18;
                vertexMesh.push_back(vertex);

                GLuint texX = leftTexcoord[t++] + ((blockType.side & zMask) >> zOffset);
                GLuint texY = leftTexcoord[t++] + (blockType.side & xMask);
                GLuint texcoords = texX | texY << 8;
                vertexMesh.push_back(texcoords);
            }
        }

        if (isAir(getBlockGlobal(x + 1, y, z))) {
            indexMesh.push_back((vertexMesh.size() / 2) + 0);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 2);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            std::array<unsigned short, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Right);
            size_t j = 0;
            size_t t = 0;
            for (size_t i = 0; i < 4; i++) {
                GLuint xi = x + rightFace[j++];
                GLuint yi = y + rightFace[j++];
                GLuint zi = z + rightFace[j++];

                GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18;
                vertexMesh.push_back(vertex);

                GLuint texX = rightTexcoord[t++] + ((blockType.side & zMask) >> zOffset);
                GLuint texY = rightTexcoord[t++] + (blockType.side & xMask);
                GLuint texcoords = texX | texY << 8;
                vertexMesh.push_back(texcoords);
            }
        }

        if (isAir(getBlockGlobal(x, y - 1, z))) {
            indexMesh.push_back((vertexMesh.size() / 2) + 0);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 2);
            indexMesh.push_back((vertexMesh.size() / 2) + 1);
            indexMesh.push_back((vertexMesh.size() / 2) + 3);
            std::array<unsigned short, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Bottom);
            size_t j = 0;
            size_t t = 0;
            for (size_t i = 0; i < 4; i++) {
                GLuint xi = x + bottomFace[j++];
                GLuint yi = y + bottomFace[j++];
                GLuint zi = z + bottomFace[j++];

                GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18;
                vertexMesh.push_back(vertex);

                GLuint texX = bottomTexcoord[t++] + ((blockType.bottom & zMask) >> zOffset);
                GLuint texY = bottomTexcoord[t++] + (blockType.bottom & xMask);
                GLuint texcoords = texX | texY << 8;
                vertexMesh.push_back(texcoords);
            }
        }
    }
    renderInit(vertexMesh, indexMesh);
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

glm::uvec3 Chunk::getPos() {
    return pos;
}

void Chunk::generateTerrain() {
    float freq = 0.96323343; // Frequency
    float amp = 0.95;        // Amplifier
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            // Uses perlin function to calculate height for xz position
            float noise =
                perlin(((x + pos.x) * freq) / 16, ((z + pos.z) * freq) / 16);
            float height = noise * amp + 1;
            for (unsigned y = 0; y < worldHeight; y++) {
                // Using the following line in an if or switch case
                // can let you dynamically decide block generations at
                // different (x, y, z) values.
                enum Block::BlockType bt;
                if (y < height) {
                    bt = Block::Grass;
                } else {
                    bt = Block::Air;
                }
                unsigned val = bt << typeOffset;
                blockArray.push_back(val);
            }
        }
    }
}

void Chunk::renderInit(std::vector<GLuint>& vertexMesh,
                       std::vector<unsigned>& indexMesh) {
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

std::array<unsigned short, 4>
Chunk::getOcclusion(unsigned x, unsigned y, unsigned z, unsigned short face) {

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
            vertexOcclusion[0] = 2;
            vertexOcclusion[3] = 2;
        }
        if (!isAir(getBlockGlobal(x, y - 1, z - 1))) {
            vertexOcclusion[0] = 3;
            vertexOcclusion[3] = 3;
        }
    } break;
    case Block::Front: {
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[1] = 2;
            vertexOcclusion[2] = 2;
        }
        if (!isAir(getBlockGlobal(x, y - 1, z + 1))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Right: {
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[1] = 2;
            vertexOcclusion[2] = 2;
        }
        if (!isAir(getBlockGlobal(x + 1, y - 1, z))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Left: {
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[1] = 2;
            vertexOcclusion[2] = 2;
        }
        if (!isAir(getBlockGlobal(x - 1, y - 1, z))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    }
    return vertexOcclusion;
}
