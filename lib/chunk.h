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
#include <cstdint>

class World;

// Fix later lol, cba rn
/*
struct BlockFaceData { 
    const MeshData*,

    unsigned x,
    unsigned y,
    unsigned z,

    unsigned transparency,
    unsigned light,
    unsigned isHighlighted
};
*/

class Chunk {
public:
    Chunk(unsigned, unsigned, World*);
    ~Chunk();

    void generateMesh();
    void draw(unsigned);

    unsigned getBlock(unsigned, unsigned, unsigned);
    std::optional<unsigned> getBlockGlobal(long, long, long);
    bool removeBlock(unsigned, unsigned, unsigned);
    bool placeBlock(Block::BlockType, unsigned, unsigned, unsigned);
    void clearMesh();
    void unhighlightBlock(unsigned, unsigned, unsigned);
    void highlightBlock(unsigned, unsigned, unsigned);
    glm::uvec3 getPos();
    bool hasLoaded();
    float distanceFrom(glm::uvec3);
    unsigned maxDistanceFrom(glm::uvec3);
    unsigned minDistanceFrom(glm::uvec3);

    std::vector<unsigned> placeTree();
    void loadStructure(const StructureData*, unsigned, unsigned, unsigned);

    Chunk* getFrontChunk();
    Chunk* getBackChunk();
    Chunk* getLeftChunk();
    Chunk* getRightChunk();

    void findAdjacentChunks();
    void transferData(std::vector<std::pair<glm::ivec3, enum Block::BlockType>>);
    void lightTraverse(size_t, unsigned char, enum Block::BlockFace = Block::FullBlock);
    void calculateLight(enum Block::BlockFace = Block::FullBlock);

private:
    void generateTerrain();
    void opaqueRenderInit();
    void transparentRenderInit();

    void loadOpaqueFace(const MeshData*, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, bool);
    void loadTransparentFace(const MeshData*, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, bool);
    void reloadAjacentChunks();

    std::array<ushort, 4> getOcclusion(unsigned, unsigned, unsigned, ushort);
    std::array<uint_fast16_t, 16 * 16 * 254> blockArray{};
    // Light values for each face of a block,
    // Top, Bottom, Left, Right, Front, Back
    std::array<std::array<uint_fast8_t, 6>, 16 * 16 * 254> lightingFaces{0};
    glm::uvec3 pos;

    World* world;

    std::vector<unsigned> oIndexMesh;
    std::vector<GLuint> oVertexMesh;

    std::vector<unsigned> tIndexMesh;
    std::vector<GLuint> tVertexMesh;

    unsigned oVAO;
    unsigned oVBO;
    unsigned oEBO;
    
    unsigned tVAO;
    unsigned tVBO;
    unsigned tEBO;

    unsigned oIndexSize;
    unsigned tIndexSize;

    bool loaded = false;

    Chunk* frontChunk = nullptr;
    Chunk* backChunk = nullptr;
    Chunk* leftChunk = nullptr;
    Chunk* rightChunk = nullptr;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> frontChunkUpdates;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> backChunkUpdates;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> leftChunkUpdates;
    std::vector<std::pair<glm::ivec3, enum Block::BlockType>> rightChunkUpdates;
};
