#include "glad/glad.h"

#include "block.h"
#include "chunk.h"
#include "perlin.h"
#include "world.h"

#include <array>
#include <glm/fwd.hpp>

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
    std::vector<float> vertexMesh;
    std::vector<unsigned> indexMesh;

    for (unsigned block : blockArray) {
        if (isAir(block)) {
            continue;
        }

        unsigned x = (block & xMask) >> xOffset;
        unsigned y = (block & yMask) >> yOffset;
        unsigned z = (block & zMask) >> zOffset;
        Block blockType{ (block & typeMask) >> blockTypeOffset };

        if (isAir(getBlockGlobal(x, y + 1, z))) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Top);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(topVertices[i] + x + pos.x);
                vertexMesh.push_back(topVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(topVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    topVertices[i + 3] +
                    static_cast<float>((blockType.top & zMask) >> zOffset));
                vertexMesh.push_back(topVertices[i + 4] +
                                     (blockType.top & xMask));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        if (isAir(getBlockGlobal(x, y, z - 1))) {
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Back);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(backVertices[i] + x + pos.x);
                vertexMesh.push_back(backVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(backVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    backVertices[i + 3] +
                    static_cast<float>((blockType.side & zMask) >> zOffset));
                vertexMesh.push_back(backVertices[i + 4] +
                                     (blockType.side & xMask));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        if (isAir(getBlockGlobal(x, y, z + 1))) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Front);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(frontVertices[i] + x + pos.x);
                vertexMesh.push_back(frontVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(frontVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    frontVertices[i + 3] +
                    static_cast<float>((blockType.side & zMask) >> zOffset));
                vertexMesh.push_back(frontVertices[i + 4] +
                                     (blockType.side & 15));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        if (isAir(getBlockGlobal(x - 1, y, z))) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Left);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(leftVertices[i] + x + pos.x);
                vertexMesh.push_back(leftVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(leftVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    leftVertices[i + 3] +
                    static_cast<float>((blockType.side & zMask) >> zOffset));
                vertexMesh.push_back(leftVertices[i + 4] +
                                     (blockType.side & xMask));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        if (isAir(getBlockGlobal(x + 1, y, z))) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Right);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(rightVertices[i] + x + pos.x);
                vertexMesh.push_back(rightVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(rightVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    rightVertices[i + 3] +
                    static_cast<float>((blockType.side & zMask) >> zOffset));
                vertexMesh.push_back(rightVertices[i + 4] +
                                     (blockType.side & xMask));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        if (isAir(getBlockGlobal(x, y - 1, z))) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(bottomVertices[i] + x + pos.x);
                vertexMesh.push_back(bottomVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(bottomVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    bottomVertices[i + 3] +
                    static_cast<float>((blockType.bottom & zMask) >> zOffset));
                vertexMesh.push_back(bottomVertices[i + 4] +
                                     (blockType.bottom & xMask));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(3);
            }
        }
    }
    renderInit(vertexMesh, indexMesh);
}

void Chunk::draw() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
}

unsigned Chunk::getBlock(unsigned x, unsigned y, unsigned z) {
    size_t ix = y + (z * worldHeight) + (x * 16 * worldHeight);
    return blockArray[ix];
}

// Signed integers to use as offset
unsigned Chunk::getBlockGlobal(long dX, long dY, long dZ) {
    if (dY < 0 || dY >= worldHeight) {
        // TODO(Christoffer): Better error block?
        return UINT32_MAX;
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
    float freq = 0.76323343; // Frequency
    float amp = 0.75;        // Amplifier
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
                unsigned val = x << xOffset | y << yOffset | z << zOffset |
                               bt << blockTypeOffset;
                blockArray.push_back(val);
            }
        }
    }
}

void Chunk::renderInit(std::vector<float>& vertexMesh,
                       std::vector<unsigned>& indexMesh) {
    indexSize = indexMesh.size();
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexMesh.size() * sizeof(float),
                 vertexMesh.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexMesh.size() * sizeof(unsigned),
                 indexMesh.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
}

std::array<float, 4>
Chunk::getOcclusion(unsigned x, unsigned y, unsigned z, unsigned short face) {

    std::array<float, 4> vertexOcclusion{ 0, 0, 0, 0 };
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
        if (!isAir(getBlockGlobal(x, y - 1, z - 1))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Front: {
        if (!isAir(getBlock(x, y - 1, z + 1))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Right: {
        if (!isAir(getBlockGlobal(x + 1, y - 1, z))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Left: {
        if (!isAir(getBlockGlobal(x - 1, y - 1, z))) {
            vertexOcclusion[2] = 3;
            vertexOcclusion[1] = 3;
        }
    } break;
    }
    return vertexOcclusion;
}
