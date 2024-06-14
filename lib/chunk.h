#pragma once

#include "mesh.h"
#include "structures.h"
#include "world.h"

#include <GL/gl.h>
#include <array>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <sys/types.h>
#include <vector>
#include <cmath>
#include <random>

class World;

class Chunk {
public:
    Chunk(unsigned, unsigned, World*);
    ~Chunk();

    void generateMesh(std::optional<std::vector<ushort>> = std::nullopt);
    void draw(unsigned);

    ushort getBlock(unsigned, unsigned, unsigned);
    std::optional<ushort> getBlockGlobal(long, long, long);
    bool removeBlock(unsigned, unsigned, unsigned);
    bool placeBlock(Block::BlockType, unsigned, unsigned, unsigned);
    void clearMesh();
    void unhighlightBlock(unsigned, unsigned, unsigned);
    void highlightBlock(unsigned, unsigned, unsigned);
    glm::uvec3 getPos();
    bool hasLoaded();
    unsigned distanceFrom(glm::uvec3);
    unsigned minDistanceFrom(glm::uvec3);

    std::vector<unsigned> placeTree();
    void loadStructure(const StructureData*, unsigned, unsigned, unsigned);

    Chunk* getFrontChunk();
    Chunk* getBackChunk();
    Chunk* getLeftChunk();
    Chunk* getRightChunk();

    void findAdjacentChunks();
    void transferData(std::vector<std::pair<glm::ivec3, enum Block::BlockType>>);

private:
    void generateTerrain();
    void renderInit();
    void loadFace(const MeshData*, unsigned, unsigned, unsigned, unsigned, bool);

    std::array<ushort, 4> getOcclusion(unsigned, unsigned, unsigned, ushort);
    std::vector<ushort> blockArray;
    glm::uvec3 pos;

    World* world;

    unsigned VAO;
    unsigned VBO;
    unsigned EBO;
    unsigned indexSize;
    bool loaded = false;

    std::vector<GLuint> vertexMesh;
    std::vector<unsigned> indexMesh;

    Chunk* frontChunk = nullptr;
    Chunk* backChunk = nullptr;
    Chunk* leftChunk = nullptr;
    Chunk* rightChunk = nullptr;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> frontChunkUpdates;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> backChunkUpdates;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> leftChunkUpdates;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> rightChunkUpdates;
};
