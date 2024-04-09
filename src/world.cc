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
            temp.push_back(Chunk{ x, 0, z });
        }
        visibleChunks.push_back(temp);
    }

    std::cout << "Creating meshes..." << std::endl;
    for (int x = 0; x < size; x++) {
        for (auto& chunk : visibleChunks[x]) {
            auto [indices, vertices] = chunk.generateMesh();
            renderer.addRendering(vertices, indices);
        }
    }
    std::cout << "Initiating rendering..." << std::endl;
    renderer.renderInit();
    std::cout << "Done!" << std::endl;
}

World::~World() {
    std::vector<std::vector<Chunk>>().swap(visibleChunks);
}

void World::draw(glm::mat4 view) {
    renderer.render(view);
}

World::Chunk::Chunk(int x, int y, int z) {
    // Chunk offset
    xPos = x * 16;
    yPos = y * 16;
    zPos = z * 16;
    generateTerrain();
}

// NOTE: Might need to actually impelent this function for memory safety (lol)
World::Chunk::~Chunk() {
    std::vector<unsigned int>().swap(blockArray);
}

bool World::Chunk::findBlock(unsigned int thisBlock) const {
    for (auto& it : blockArray) {
        if ((thisBlock & ~blockTypeBits) ==
            (it & ~blockTypeBits)) {
            return true;
        }
    }
    return false;
}

std::pair<std::vector<unsigned int>, std::vector<float>>
World::Chunk::generateMesh() {
    // NOTE: This implementation is quite slow, but very consitent.
    // It will also be easy to modify whenever normals are to be added, however
    // it can 100% be cleaned up and become even better
    // (with less if statements, if possible here).
    std::vector<unsigned int> indexMesh;
    std::vector<float> vertexMesh;
    for (unsigned int it = 0; it < blockArray.size(); it++) {
        unsigned int block = blockArray[it];

        unsigned int x = (block & xBits) / xDivision;
        unsigned int y = (block & yBits) / yDivision;
        unsigned int z = (block & zBits) / zDivision;
        Block blockType((block & blockTypeBits) / blockTypeDivision);

        if (!obsTop(it)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(topVertices[i] + x + xPos);
                vertexMesh.push_back(topVertices[i + 1] + y + yPos);
                vertexMesh.push_back(topVertices[i + 2] + z + zPos);
                vertexMesh.push_back(topVertices[i + 3] +
                                     static_cast<float>(blockType.top & zBits) /
                                         zDivision);
                vertexMesh.push_back(topVertices[i + 4] + (blockType.top & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(0.0);
            }
        }

        if (!obsBack(it) && z != 0) {
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(backVertices[i] + x + xPos);
                vertexMesh.push_back(backVertices[i + 1] + y + yPos);
                vertexMesh.push_back(backVertices[i + 2] + z + zPos);
                vertexMesh.push_back(backVertices[i + 3] +
                                     static_cast<float>(blockType.side & zBits) /
                                         zDivision);
                vertexMesh.push_back(backVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(1.0);// Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }
        if (!obsFront(it) && z != 15) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(frontVertices[i] + x + xPos);
                vertexMesh.push_back(frontVertices[i + 1] + y + yPos);
                vertexMesh.push_back(frontVertices[i + 2] + z + zPos);
                vertexMesh.push_back(frontVertices[i + 3] +
                                     static_cast<float>(blockType.side & zBits) /
                                         zDivision);
                vertexMesh.push_back(frontVertices[i + 4] +
                                     (blockType.side & 15));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(1.0);// Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }
        if (!obsLeft(it) && x != 0) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(leftVertices[i] + x + xPos);
                vertexMesh.push_back(leftVertices[i + 1] + y + yPos);
                vertexMesh.push_back(leftVertices[i + 2] + z + zPos);
                vertexMesh.push_back(leftVertices[i + 3] +
                                     static_cast<float>(blockType.side & zBits) /
                                         zDivision);
                vertexMesh.push_back(leftVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(1.0);// Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }
        if (!obsRight(it) && x != 15) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(rightVertices[i] + x + xPos);
                vertexMesh.push_back(rightVertices[i + 1] + y + yPos);
                vertexMesh.push_back(rightVertices[i + 2] + z + zPos);
                vertexMesh.push_back(rightVertices[i + 3] +
                                     static_cast<float>(blockType.side & zBits) /
                                         zDivision);
                vertexMesh.push_back(rightVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                if (i == 16 || i == 8) {
                    vertexMesh.push_back(1.0);// Occlusion value, make function for this.
                } else {
                    vertexMesh.push_back(0.0);
                }
            }
        }
        if (!obsBottom(it) && y != 0) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(bottomVertices[i] + x + xPos);
                vertexMesh.push_back(bottomVertices[i + 1] + y + yPos);
                vertexMesh.push_back(bottomVertices[i + 2] + z + zPos);
                vertexMesh.push_back(
                    bottomVertices[i + 3] +
                    static_cast<float>(blockType.bottom & zBits) / zDivision);
                vertexMesh.push_back(bottomVertices[i + 4] +
                                     (blockType.bottom & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(1.0);// Occlusion value, make function for this.
            }
        }
    }
    return std::make_pair(indexMesh, vertexMesh);
}

void World::Chunk::generateTerrain() {
    float freq = 0.76323343; // Frequency
    float amp = 0.75;        // Amplifier
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            // Uses perlin function to calculate height for xz position
            float noise =
                perlin(((x + xPos) * freq) / 16, ((z + zPos) * freq) / 16);
            float height = noise * amp + 1;
            for (int y = 0; y < height; y++) {
                unsigned int val = (((0 | x) | z * zDivision) | y * yDivision);
                // Using the following line in an if or switch case
                // can let you dynamically decide block generations at
                // different (x, y, z) values.
                val = (val | 0 * blockTypeDivision);
                blockArray.push_back(val);
            }
        }
    }
}

// z
bool World::Chunk::obsBack(unsigned int thisBlock) {
    if (thisBlock == 0) {
        return false;
    }
    int temp = thisBlock - 1;
    while ((blockArray[temp] & zBits) >=
               ((blockArray[thisBlock] & zBits) - zDivision) &&
           temp != -1) {
        if ((blockArray[temp] & ~zBits) ==
            (blockArray[thisBlock] & ~zBits)) {
            return true;
        }
        temp--;
    }
    return false;
}

// z
bool World::Chunk::obsFront(unsigned int thisBlock) {
    if (blockArray[thisBlock] == blockArray.back()) {
        return false;
    }
    int temp = thisBlock + 1;
    while ((blockArray[temp] & zBits) <=
               ((blockArray[thisBlock] & zBits) + zDivision) &&
           blockArray[temp - 1] != blockArray.back()) {
        if ((blockArray[temp] & ~zBits) ==
            (blockArray[thisBlock] & ~zBits)) {
            return true;
        }
        temp++;
    }
    return false;
}

// x
bool World::Chunk::obsLeft(unsigned int thisBlock) {
    if (thisBlock == 0) {
        return false;
    }
    int temp = thisBlock - 1;
    while ((blockArray[temp] & xBits) >=
               ((blockArray[thisBlock] & xBits) - xDivision) &&
           temp != -1) {
        if ((blockArray[temp] & ~xBits) ==
            (blockArray[thisBlock] & ~xBits)) {
            return true;
        }
        temp--;
    }
    return false;
}

// x
bool World::Chunk::obsRight(unsigned int thisBlock) {
    if (blockArray[thisBlock] == blockArray.back()) {
        return false;
    }
    int temp = thisBlock + 1;
    while ((blockArray[temp] & xBits) <=
               ((blockArray[thisBlock] & xBits) + xDivision) &&
           blockArray[temp - 1] != blockArray.back()) {
        if ((blockArray[temp] & ~xBits) ==
            (blockArray[thisBlock] & ~xBits)) {
            return true;
        }
        temp++;
    }
    return false;
}

// y
bool World::Chunk::obsTop(unsigned int thisBlock) {
    if (blockArray[thisBlock] == blockArray.back()) {
        return false;
    }
    return (((blockArray[thisBlock + 1] & yBits)) ==
            ((blockArray[thisBlock] & yBits) + yDivision));
}

// y
bool World::Chunk::obsBottom(unsigned int thisBlock) {
    if (thisBlock == 0) {
        return false;
    }
    return (((blockArray[thisBlock - 1] & yBits)) ==
            ((blockArray[thisBlock] & yBits) - yDivision));
}
