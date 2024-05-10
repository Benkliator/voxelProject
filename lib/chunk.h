#pragma once

#include "mesh.h"
#include "world.h"

#include <GL/gl.h>
#include <array>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <sys/types.h>
#include <vector>

class World;

class Chunk {
public:
    Chunk(int, int, World*);
    ~Chunk();

    void generateMesh(std::optional<std::vector<ushort>> = std::nullopt);
    void reloadMesh(unsigned, unsigned, unsigned);
    void draw(unsigned);

    ushort getBlock(unsigned, unsigned, unsigned);
    std::optional<ushort> getBlockGlobal(long, long, long);
    bool removeBlock(unsigned, unsigned, unsigned);
    bool removeBlockMesh(unsigned, unsigned, unsigned);
    bool placeBlock(Block::BlockType, unsigned, unsigned, unsigned);
    void clearMesh();
    void unhighlightBlock(unsigned, unsigned, unsigned);
    void highlightBlock(unsigned, unsigned, unsigned);
    glm::uvec3 getPos();
    bool hasLoaded();

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
};
