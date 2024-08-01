#include "glad/glad.h"

#include "block.h"
#include "chunk.h"
#include "noise.h"
#include "world.h"

#include <array>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optional>
#include <sys/types.h>
#include <iostream>

// Generates buffers and VAO, creates terrain for chunk.
Chunk::Chunk(unsigned x, unsigned z, World* w) : world{ w } {
    // Chunk offset
    glGenVertexArrays(1, &oVAO);
    glGenBuffers     (1, &oVBO);
    glGenBuffers     (1, &oEBO);

    glGenVertexArrays(1, &tVAO);
    glGenBuffers     (1, &tVBO);
    glGenBuffers     (1, &tEBO);

    pos = {
        x * 16,
        0,
        z * 16,
    };
    generateTerrain();
}

Chunk::~Chunk() {}

bool Chunk::hasLoaded() {
    return loaded;
}

void Chunk::generateMesh(std::optional<std::vector<unsigned>> blockAreaArray) {
    calculateLight();
    std::vector<unsigned> blocks = blockAreaArray.value_or(this->blockArray);
    findAdjacentChunks();
    const unsigned obstruct = Block::Dirt << typeOffset;
    for (size_t i = 0; i < blocks.size(); i++) {
        unsigned block = blocks[i];
        if (isAir(block)) {
            continue;
        }
        unsigned z = i / (worldHeight * 16);
        unsigned y = i % worldHeight;
        unsigned x = (i % (16 * worldHeight)) / worldHeight;
        Block blockType{
            static_cast<ushort>((block & typeMask) >> typeOffset),
        };
        unsigned light = ((block >> 16) & 15);

        bool top;
        bool bottom;
        bool back;
        bool front;
        bool left;
        bool right;
        if (x >= 1 && x <= 14 && z >= 1 && z <= 14) {
            top = isTransparent(getBlock(x, y + 1, z), block);
            if (y != 0) {
                bottom = isTransparent(getBlock(x, y - 1, z), block);
            } else {
                bottom = false;
            }
            back =  isTransparent(getBlock(x, y, z - 1), block);
            front = isTransparent(getBlock(x, y, z + 1), block);
            left =  isTransparent(getBlock(x - 1, y, z), block);
            right = isTransparent(getBlock(x + 1, y, z), block);
        } else {
            top =    isTransparent(getBlockGlobal(x, y + 1, z).value_or(obstruct), block);
            bottom = isTransparent(getBlockGlobal(x, static_cast<long>(y) - 1, z).value_or(obstruct), block);
            back =   isTransparent(getBlockGlobal(x, y, static_cast<long>(z) - 1).value_or(obstruct), block);
            front =  isTransparent(getBlockGlobal(x, y, z + 1).value_or(obstruct), block);
            left =   isTransparent(getBlockGlobal(static_cast<long>(x) - 1, y, z).value_or(obstruct), block);
            right =  isTransparent(getBlockGlobal(x + 1, y, z).value_or(obstruct), block);
        }
        if (top) {
            blockArray[i] |= topMask;
            if (blockType.transparency) {
                loadTransparentFace(&topMeshData, blockType.top, x, y, z, blockType.transparency, light, block & 1);
            } else {
                loadOpaqueFace(&topMeshData, blockType.top, x, y, z, blockType.transparency, light, block & 1);
            }
        }

        if (bottom) {
            blockArray[i] |= bottomMask;
            if (blockType.transparency) {
                loadTransparentFace(&bottomMeshData, blockType.bottom, x, y, z, blockType.transparency, light, block & 1);
            } else {
                loadOpaqueFace(&bottomMeshData, blockType.bottom, x, y, z, blockType.transparency, light, block & 1);
            }
        }

        if (back) {
            blockArray[i] |= backMask;
            if (blockType.transparency) {
                loadTransparentFace(&backMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            } else {
                loadOpaqueFace(&backMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            }
        }

        if (front) {
            blockArray[i] |= frontMask;
            if (blockType.transparency) {
                loadTransparentFace(&frontMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            } else {
                loadOpaqueFace(&frontMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            }
        }
        if (left) {
            blockArray[i] |= leftMask;
            if (blockType.transparency) {
                loadTransparentFace(&leftMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            } else {
                loadOpaqueFace(&leftMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            }
        }

        if (right) {
            blockArray[i] |= rightMask;
            if (blockType.transparency) {
                loadTransparentFace(&rightMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            } else {
                loadOpaqueFace(&rightMeshData, blockType.side, x, y, z, blockType.transparency, light, block & 1);
            }
        }
    }
    if (!oIndexMesh.empty()) {
        opaqueRenderInit();
    }
    if (!tIndexMesh.empty()) {
        transparentRenderInit();
    }
    loaded = true;
}

void Chunk::draw(unsigned shader) {
    glUniform3uiv(
        glGetUniformLocation(shader, "chunkPos"), 1, glm::value_ptr(pos));

    if (!oIndexMesh.empty()) {
        glBindVertexArray(oVAO);
        glDrawElements(GL_TRIANGLES, oIndexMesh.size(), GL_UNSIGNED_INT, 0);
    }
    if (!tIndexMesh.empty()) {
        glBindVertexArray(tVAO);
        glDrawElements(GL_TRIANGLES, tIndexMesh.size(), GL_UNSIGNED_INT, 0);
    }
}

unsigned Chunk::getBlock(unsigned x, unsigned y, unsigned z) {
    size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
    return blockArray[ix];
}

std::optional<unsigned> Chunk::getBlockGlobal(long dX, long dY, long dZ) {
    if (dY < 0 || dY >= worldHeight) {
        return std::nullopt;
    }
    if (dX >= 0 && dX <= 15 && dZ >= 0 && dZ <= 15) {
        // Can use local version
        return getBlock(dX, dY, dZ);
    } else {
        if (frontChunk && dZ == 16 && dX < 16 && dX > -1) {
            return frontChunk->getBlock(dX, dY, 0);
        } else if (backChunk && dZ == -1 && dX < 16 && dX > -1) {
            return backChunk->getBlock(dX, dY, 15);
        }

        if (leftChunk && dX == -1 && dZ < 16 && dZ > -1) {
            return leftChunk->getBlock(15, dY, dZ);
        } else if (rightChunk && dX == 16 && dZ < 16 && dZ > -1) {
            return rightChunk->getBlock(0, dY, dZ);
        }

        // Assume that there will always be a configured adjacent chunk, else:
        //return std::nullopt;
        // Maybe not?
        return world->getBlock(pos.x + dX, pos.y + dY, pos.z + dZ);
    }
}

bool Chunk::removeBlock(unsigned x, unsigned y, unsigned z) {
    if (!y) {
        return false;
    }
    if (!isAir(getBlock(x, y, z))) {
        size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
        blockArray[ix] = 0;
        clearMesh();
        generateMesh();

        if (frontChunk && z == 15) {
            frontChunk->clearMesh();
            frontChunk->generateMesh();
        } else if (backChunk && z == 0) {
            backChunk->clearMesh();
            backChunk->generateMesh();
        }
        if (rightChunk && x == 15) {
            rightChunk->clearMesh();
            rightChunk->generateMesh();
        } else if (leftChunk && x == 0) {
            leftChunk->clearMesh();
            leftChunk->generateMesh();
        }
        // reloadMesh(x, y, z);
        return true;
    };
    return false;
}

bool Chunk::placeBlock(Block::BlockType bt,
                       unsigned x,
                       unsigned y,
                       unsigned z) {
    unsigned block = getBlock(x, y, z);
    if (isAir(block)) {
        size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
        blockArray[ix] = bt << typeOffset;
        clearMesh();
        generateMesh();
        if (frontChunk && z == 15) {
            frontChunk->clearMesh();
            frontChunk->generateMesh();
        } else if (backChunk && z == 0) {
            backChunk->clearMesh();
            backChunk->generateMesh();
        }
        if (rightChunk && x == 15) {
            rightChunk->clearMesh();
            rightChunk->generateMesh();
        } else if (leftChunk && x == 0) {
            leftChunk->clearMesh();
            leftChunk->generateMesh();
        }
        // reloadMesh(x, y, z);
        return true;
    };
    return false;
}

void Chunk::clearMesh() {
    oVertexMesh.clear();
    oIndexMesh.clear();

    tVertexMesh.clear();
    tIndexMesh.clear();
}

void Chunk::unhighlightBlock(unsigned x, unsigned y, unsigned z) {
    size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
    blockArray[ix] = blockArray[ix] & ~1;
}

void Chunk::highlightBlock(unsigned x, unsigned y, unsigned z) {
    size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
    blockArray[ix] = blockArray[ix] | 1;
}

glm::uvec3 Chunk::getPos() {
    return pos;
}

void Chunk::generateTerrain() {
    float yIntercept = 30.0f;
    float freq = 0.8396323343; // Frequency
    float amp = 1.35;          // Amplifier
    std::vector<int> heightmap;
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            // Uses perlin function to calculate height for xz position
            float noise =
                perlin(((z + pos.x) * freq) / 16, ((x + pos.z) * freq) / 16);
            float height = noise * amp + yIntercept;
            for (unsigned y = 0; y < worldHeight; ++y) {
                // Using the following line in an if or switch case
                // can let you dynamically decide block generations at
                // different (x, y, z) values.
                enum Block::BlockType bt;
                if (y <= 1 + yIntercept && !(y < height)) {
                    bt = Block::Water;
                    heightmap.push_back(-1);
                } else if (y > height && y < height + 1) {
                    bt = Block::Grass;
                    heightmap.push_back(y);
                } else if (y > (height - 3) && y < height) {
                    bt = Block::Dirt;
                } else if (y <= height - 3){
                    bt = Block::Stone;
                } else {
                    bt = Block::Air;
                }
                unsigned block = bt << typeOffset;
                blockArray.push_back(block);
            }
        }
    }

    std::vector<unsigned> treeHeight = placeTree();
    unsigned i = 0;
    for (unsigned z = 0; z < 16; ++z) {
        for (unsigned x = 0; x < 16; ++x) {
            enum Block::BlockType bt = Block::Log;
            if ((heightmap[i] != -1)) {
                int y = heightmap[i];
                int ix = y + 1 + (z * 16 * worldHeight) + (x * worldHeight);
                for (int j = 0; j < treeHeight[i]; ++j) {
                    unsigned block = bt << typeOffset;
                    blockArray[ix + j] = block;
                }
                if (treeHeight[i]) {
                    loadStructure(&treeLeaves, x, treeHeight[i] + y, z);
                }
            }
            ++i;
        }
    }
}

void Chunk::loadStructure(const StructureData* structure, unsigned x, unsigned y, unsigned z) {
    for (auto [updatePos, bt] : structure->data) {
        int nx = updatePos.x + x;
        int ny = updatePos.y + y;
        int nz = updatePos.z + z;
        if (nx > 15) {
            rightChunkUpdates.emplace_back(glm::ivec3(nx - 16, ny, nz), bt);
        } else if (nx < 0) {
            leftChunkUpdates.emplace_back(glm::ivec3(nx + 16, ny, nz), bt);
        } else if (nz > 15) {
            frontChunkUpdates.emplace_back(glm::ivec3(nx, ny, nz - 16), bt);
        } else if (nz < 0) {
            backChunkUpdates.emplace_back(glm::ivec3(nx, ny, nz + 16), bt);
        } else {
            size_t ix = ny + (nz * 16 * worldHeight) + (nx * worldHeight);
            unsigned block = bt << typeOffset;
            blockArray[ix] = block;
        }
    }
    //generateMesh();
}

std::vector<unsigned> Chunk::placeTree() {
    std::random_device rd; // Seed for "random" number generation.
    std::mt19937 gen( rd() );
    std::uniform_int_distribution<> treeRoll(0, 100);
    std::uniform_int_distribution<> treeLength(3, 4);

    std::vector<unsigned> result;
    
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            unsigned treeResult = treeRoll(gen);
            if (treeResult == 1) {
                 result.push_back(treeLength(gen));
            } else {
                result.push_back(0);
            }
        }
    }
    return result;
}

void Chunk::opaqueRenderInit() {
    // Opaque rendering
    oIndexSize = oIndexMesh.size();
    glBindVertexArray(oVAO);

    glBindBuffer(GL_ARRAY_BUFFER, oVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 oVertexMesh.size() * sizeof(GLuint),
                 oVertexMesh.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 oIndexMesh.size() * sizeof(unsigned),
                 oIndexMesh.data(),
                 GL_STATIC_DRAW);

    glVertexAttribIPointer(
        0, 2, GL_UNSIGNED_INT, 2 * sizeof(GLuint), (void*)(0));
    glEnableVertexAttribArray(0);
    
}

void Chunk::transparentRenderInit() {
    // Transparent rendering
    oIndexSize = oIndexMesh.size();
    glBindVertexArray(tVAO);

    glBindBuffer(GL_ARRAY_BUFFER, tVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 tVertexMesh.size() * sizeof(GLuint),
                 tVertexMesh.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 tIndexMesh.size() * sizeof(unsigned),
                 tIndexMesh.data(),
                 GL_STATIC_DRAW);

    glVertexAttribIPointer(
        0, 2, GL_UNSIGNED_INT, 2 * sizeof(GLuint), (void*)(0));
    glEnableVertexAttribArray(0);
}

void Chunk::loadOpaqueFace(const MeshData* data,
                     unsigned bt,
                     unsigned x,
                     unsigned y,
                     unsigned z,
                     unsigned transparency,
                     unsigned light,
                     bool highlight) {
    oIndexMesh.push_back((oVertexMesh.size() / 2) + 0);
    oIndexMesh.push_back((oVertexMesh.size() / 2) + 3);
    oIndexMesh.push_back((oVertexMesh.size() / 2) + 1);
    oIndexMesh.push_back((oVertexMesh.size() / 2) + 2);
    oIndexMesh.push_back((oVertexMesh.size() / 2) + 1);
    oIndexMesh.push_back((oVertexMesh.size() / 2) + 3);
    std::array<ushort, 4> occlusionArray =
        getOcclusion(x, y, z, data->blockFace);
    size_t j = 0;
    size_t t = 0;
    for (size_t i = 0; i < 4; i++) {
        GLuint xi = x + data->faceCoords[j++];
        GLuint yi = y + data->faceCoords[j++];
        GLuint zi = z + data->faceCoords[j++];
        GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18 |
                        static_cast<unsigned>(highlight) << 20;

        GLuint texX = data->texCoords[t++] + ((bt & zMask) >> zOffset);
        GLuint texY = data->texCoords[t++] + ((bt & xMask) >> xOffset);
        GLuint texData = texX | texY << 8 | transparency << 16 | light << 18;

        oVertexMesh.push_back(vertex);
        oVertexMesh.push_back(texData);
    }
}

void Chunk::loadTransparentFace(const MeshData* data,
                     unsigned bt,
                     unsigned x,
                     unsigned y,
                     unsigned z,
                     unsigned transparency,
                     unsigned light,
                     bool highlight) {
    tIndexMesh.push_back((tVertexMesh.size() / 2) + 0);
    tIndexMesh.push_back((tVertexMesh.size() / 2) + 3);
    tIndexMesh.push_back((tVertexMesh.size() / 2) + 1);
    tIndexMesh.push_back((tVertexMesh.size() / 2) + 2);
    tIndexMesh.push_back((tVertexMesh.size() / 2) + 1);
    tIndexMesh.push_back((tVertexMesh.size() / 2) + 3);
    std::array<ushort, 4> occlusionArray =
        getOcclusion(x, y, z, data->blockFace);
    size_t j = 0;
    size_t t = 0;
    for (size_t i = 0; i < 4; i++) {
        GLuint xi = x + data->faceCoords[j++];
        GLuint yi = y + data->faceCoords[j++];
        GLuint zi = z + data->faceCoords[j++];
        GLuint vertex = xi | yi << 5 | zi << 13 | occlusionArray[i] << 18 |
                        static_cast<unsigned>(highlight) << 20;

        GLuint texX = data->texCoords[t++] + ((bt & zMask) >> zOffset);
        GLuint texY = data->texCoords[t++] + ((bt & xMask) >> xOffset);
        GLuint texData = texX | texY << 8 | transparency << 16 | light << 18;

        tVertexMesh.push_back(vertex);
        tVertexMesh.push_back(texData);
    }
}

std::array<ushort, 4>
Chunk::getOcclusion(unsigned x, unsigned y, unsigned z, ushort face) {
    // This is really ugly lol, will probably rework at some point.
    std::array<ushort, 4> vertexOcclusion{ 0, 0, 0, 0 };
    const ushort air = Block::Air << typeOffset;
    switch (face) {
    case Block::Top: {
        if (!isAir(getBlockGlobal(x, y + 1, z + 1).value_or(air))) {
            vertexOcclusion[1] += 1;
            vertexOcclusion[2] += 1;
        }
        if (!isAir(getBlockGlobal(x, y + 1, z - 1).value_or(air))) {
            vertexOcclusion[0] += 1;
            vertexOcclusion[3] += 1;
        }
        if (!isAir(getBlockGlobal(x + 1, y + 1, z).value_or(air))) {
            vertexOcclusion[0] += 1;
            vertexOcclusion[1] += 1;
        }
        if (!isAir(getBlockGlobal(x - 1, y + 1, z).value_or(air))) {
            vertexOcclusion[2] += 1;
            vertexOcclusion[3] += 1;
        }
        if (!isAir(getBlockGlobal(x + 1, y + 1, z + 1).value_or(air))) {
            vertexOcclusion[1] += 1;
        }
        if (!isAir(getBlockGlobal(x - 1, y + 1, z + 1).value_or(air))) {
            vertexOcclusion[2] += 1;
        }
        if (!isAir(getBlockGlobal(x + 1, y + 1, z - 1).value_or(air))) {
            vertexOcclusion[0] += 1;
        }
        if (!isAir(getBlockGlobal(x - 1, y + 1, z - 1).value_or(air))) {
            vertexOcclusion[3] += 1;
        }
    } break;
    case Block::Back: {
        vertexOcclusion[0] = 1;
        vertexOcclusion[1] = 1;
        vertexOcclusion[2] = 1;
        vertexOcclusion[3] = 1;
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[0] = 1;
            vertexOcclusion[3] = 1;
        }
        if (!isAir(getBlock(x, y + 1, z))) {
            vertexOcclusion[1] = 1;
            vertexOcclusion[2] = 1;
        }
        if (!isAir(getBlockGlobal(x, y - 1, z - 1).value_or(air))) {
            vertexOcclusion[0] = 3;
            vertexOcclusion[3] = 3;
        }
    } break;
    case Block::Front: {
        vertexOcclusion[0] = 1;
        vertexOcclusion[1] = 1;
        vertexOcclusion[2] = 1;
        vertexOcclusion[3] = 1;
        if (!isAir(getBlock(x, y - 1, z))) {
            vertexOcclusion[1] = 1;
            vertexOcclusion[2] = 1;
        }
        if (!isAir(getBlock(x, y + 1, z))) {
            vertexOcclusion[0] = 1;
            vertexOcclusion[3] = 1;
        }
        if (!isAir(getBlockGlobal(x, y - 1, z + 1).value_or(air))) {
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
        if (!isAir(getBlockGlobal(x + 1, y - 1, z).value_or(air))) {
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
        if (!isAir(getBlockGlobal(x - 1, y - 1, z).value_or(air))) {
            vertexOcclusion[1] = 3;
            vertexOcclusion[2] = 3;
        }
    } break;
    case Block::Bottom: {
        vertexOcclusion[0] = 2;
        vertexOcclusion[1] = 2;
        vertexOcclusion[2] = 2;
        vertexOcclusion[3] = 2;
    }
    }
    return vertexOcclusion;
}

float Chunk::distanceFrom(glm::uvec3 point) {
    return std::sqrt(std::pow(static_cast<int>(point.x) - static_cast<int>(pos.x), 2) + std::pow(static_cast<int>(point.z) - static_cast<int>(pos.z), 2));
}

unsigned Chunk::maxDistanceFrom(glm::uvec3 point) {
    return std::max(abs(static_cast<int>(point.x) - static_cast<int>(pos.x)),
                    abs(static_cast<int>(point.z) - static_cast<int>(pos.z)));
}

unsigned Chunk::minDistanceFrom(glm::uvec3 point) {
    return std::min(abs(static_cast<int>(point.x) - static_cast<int>(pos.x)),
                    abs(static_cast<int>(point.z) - static_cast<int>(pos.z)));
}



Chunk* Chunk::getFrontChunk() {
    return frontChunk;
}

Chunk* Chunk::getBackChunk() {
    return backChunk;
}

Chunk* Chunk::getLeftChunk() {
    return leftChunk;
}

Chunk* Chunk::getRightChunk() {
    return rightChunk;
}

void Chunk::findAdjacentChunks() {
    if (!frontChunk) {
        auto searchChunk = world->getChunkSlow(pos.x, 0, pos.z + 16);
        if (searchChunk.has_value()) {
            frontChunk = &searchChunk.value().get();
            if (!frontChunkUpdates.empty()) {
                frontChunk->transferData(frontChunkUpdates);
                frontChunkUpdates.clear();
                frontChunk->generateMesh();
            }
        }
    } else {
        if (!frontChunkUpdates.empty()) {
            frontChunk->transferData(frontChunkUpdates);
            frontChunkUpdates.clear();
        }
    }

    if (!backChunk) {
        auto searchChunk = world->getChunkSlow(pos.x, 0, pos.z - 16);
        if (searchChunk.has_value()) {
            backChunk = &searchChunk.value().get();
            if (!backChunkUpdates.empty()) {
                backChunk->transferData(backChunkUpdates);
                backChunkUpdates.clear();
                backChunk->generateMesh();
            }
        }
    } else {
        if (!backChunkUpdates.empty()) {
            backChunk->transferData(backChunkUpdates);
            backChunkUpdates.clear();
        }
    }

    if (!leftChunk) {
        auto searchChunk = world->getChunkSlow(pos.x - 16, 0, pos.z);
        if (searchChunk.has_value()) {
            leftChunk = &searchChunk.value().get();
            if (!leftChunkUpdates.empty()) {
                leftChunk->transferData(leftChunkUpdates);
                leftChunkUpdates.clear();
                leftChunk->generateMesh();
            }
        }
    } else {
        if (!leftChunkUpdates.empty()) {
            leftChunk->transferData(leftChunkUpdates);
            leftChunkUpdates.clear();
        }
    }

    if (!rightChunk) {
        auto searchChunk = world->getChunkSlow(pos.x + 16, 0, pos.z);
        if (searchChunk.has_value()) {
            rightChunk = &searchChunk.value().get();
            if (!rightChunkUpdates.empty()) {
                rightChunk->transferData(rightChunkUpdates);
                rightChunkUpdates.clear();
                rightChunk->generateMesh();
            }
        }
    } else {
        if (!rightChunkUpdates.empty()) {
            rightChunk->transferData(rightChunkUpdates);
            rightChunkUpdates.clear();
        }
    }
}

void Chunk::transferData(std::vector<std::pair<glm::ivec3, enum Block::BlockType>> receivedUpdate) {
    const StructureData temp {receivedUpdate};
    loadStructure(&temp, 0, 0, 0);
}

// TODO:
// Do some sort of BFS to determine the light of surrounding blocks,
// each block away should have -1 light value of the prior.
//
// Also make sure light emitted from a block is removed whenever
// that block is destroyed.
void Chunk::calculateLight() {
    unsigned size = blockArray.size();
    for (size_t i = 0; i < size; ++i) {
        unsigned block = blockArray[i];
        Block blockType{
            static_cast<ushort>((block & typeMask) >> typeOffset),
        };

        if (blockType.glowValue) {
            blockArray[i] |= (blockType.glowValue << 16);

            // size_t ix = y + (z * 16 * worldHeight) + (x * worldHeight);
            size_t ix = i + (16 * worldHeight) - 1;
            blockArray[ix] |= ((blockType.glowValue - 4) << 16);
            ix += (16 * worldHeight);
            blockArray[ix] |= ((blockType.glowValue - 8) << 16);
            ix += (16 * worldHeight);
            blockArray[ix] |= ((blockType.glowValue - 15) << 16);
        } else {
            blockArray[i] |= (5 << 16);
        }
    }
}
