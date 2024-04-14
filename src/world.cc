#include "world.h"
#include "block.h"
#include "perlin.h"

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

World::World(unsigned int size) {
    std::cout << "Creating chunks..." << std::endl;

    for (int x = 0; x < size; x++) {
        std::vector<Chunk> temp;
        for (int z = 0; z < size; z++) {
            visibleChunks.push_back(Chunk{ x, 0, z });
        }
    }

    std::cout << "Creating meshes..." << std::endl;
    for (auto& chunk : visibleChunks) {
        auto [indices, vertices] = chunk.generateMesh();
        renderer.addRendering(vertices, indices);
    }
    std::cout << "Initiating rendering..." << std::endl;
    renderer.renderInit();
    std::cout << "Done!" << std::endl;
}

World::~World() {
    std::vector<Chunk>().swap(visibleChunks);
}

void World::draw(glm::mat4 view) {
    renderer.render(view);
}

World::Chunk::Chunk(int x, int y, int z) {
    // Chunk offset
    pos.x = x * 16;
    pos.y = y * 16;
    pos.z = z * 16;
    generateTerrain();
}

// NOTE: Might need to actually impelent this function for memory safety (lol)
World::Chunk::~Chunk() {
    std::vector<unsigned int>().swap(blockArray);
}

std::pair<std::vector<unsigned int>, std::vector<float>>
World::Chunk::generateMesh() {
    // NOTE: This implementation is quite slow, but very consitent.
    // it can 100% be cleaned up and become even better
    std::vector<unsigned int> indexMesh;
    std::vector<float> vertexMesh;
    for (unsigned int it = 0; it < blockArray.size(); it++) {
        unsigned int block = blockArray[it];

        unsigned int x = (block & xBits) / xDivision;
        unsigned int y = (block & yBits) / yDivision;
        unsigned int z = (block & zBits) / zDivision;
        Block blockType((block & blockTypeBits) / blockTypeDivision);
        if ((block & blockTypeBits) == Block::Air) {
            continue;
        }

        // (!obsTop(it))
        if (y < worldHeight &&
            ((getBlock(x, y + 1, z) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(topVertices[i] + x + pos.x);
                vertexMesh.push_back(topVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(topVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(topVertices[i + 3] +
                                     static_cast<float>(blockType.top & zBits) /
                                         zDivision);
                vertexMesh.push_back(topVertices[i + 4] +
                                     (blockType.top & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(0.0);
            }
        }

        // (!obsBack(it) && z != 0)
        if ((z > 0 && ((getBlock(x, y, z - 1) & blockTypeBits) == Block::Air))) {
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(backVertices[i] + x + pos.x);
                vertexMesh.push_back(backVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(backVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    backVertices[i + 3] +
                    static_cast<float>(blockType.side & zBits) / zDivision);
                vertexMesh.push_back(backVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(
                        1.0); // Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }

        // (!obsFront(it) && z != 15)
        if (z < 15 && ((getBlock(x, y, z + 1) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(frontVertices[i] + x + pos.x);
                vertexMesh.push_back(frontVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(frontVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    frontVertices[i + 3] +
                    static_cast<float>(blockType.side & zBits) / zDivision);
                vertexMesh.push_back(frontVertices[i + 4] +
                                     (blockType.side & 15));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(
                        1.0); // Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }

        // (!obsLeft(it) && x != 0)
        if (x > 0 && ((getBlock(x - 1, y, z) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(leftVertices[i] + x + pos.x);
                vertexMesh.push_back(leftVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(leftVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    leftVertices[i + 3] +
                    static_cast<float>(blockType.side & zBits) / zDivision);
                vertexMesh.push_back(leftVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(
                        1.0); // Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }

        // (!obsRight(it) && x != 15)
        if (x < 15 && ((getBlock(x + 1, y, z) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(rightVertices[i] + x + pos.x);
                vertexMesh.push_back(rightVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(rightVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    rightVertices[i + 3] +
                    static_cast<float>(blockType.side & zBits) / zDivision);
                vertexMesh.push_back(rightVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(
                        1.0); // Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }

        // (!obsBottom(it) && y != 0)
        if (y > 0 && ((getBlock(x, y - 1, z) & blockTypeBits) == Block::Air)) {
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
                    static_cast<float>(blockType.bottom & zBits) / zDivision);
                vertexMesh.push_back(bottomVertices[i + 4] +
                                     (blockType.bottom & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(
                    1.0); // Occlusion value, make function for this.
            }
        }
    }
    return std::make_pair(indexMesh, vertexMesh);
}

// TODO(Christoffer): This function should create a World::getBlock and use with
//                    the chunk's world pointer instead
unsigned int
World::Chunk::getBlock(unsigned int x, unsigned int y, unsigned int z) {
    size_t ix = y + (z * worldHeight) + (x * 16 * worldHeight);
    return blockArray[ix];
}

chunkPos World::Chunk::getPos() {
    return pos;
}

void World::Chunk::generateTerrain() {
    float freq = 0.76323343; // Frequency
    float amp = 0.75;        // Amplifier
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            // Uses perlin function to calculate height for xz position
            float noise =
                perlin(((x + pos.x) * freq) / 16, ((z + pos.z) * freq) / 16);
            float height = noise * amp + 1;
            for (int y = 0; y < worldHeight; y++) {
                unsigned int val = (((0 | x) | z * zDivision) | y * yDivision);
                // Using the following line in an if or switch case
                // can let you dynamically decide block generations at
                // different (x, y, z) values.
                enum Block::BlockType bt;
                if (y < height) {
                    bt = Block::Grass;
                } else {
                    bt = Block::Air;
                }
                val = (val | bt * blockTypeDivision);
                blockArray.push_back(val);
            }
        }
    }
}
