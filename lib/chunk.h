#pragma once

#include "world.h"
#include "mesh.h"

#include <array>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>

class World;

class Chunk {
public:
    Chunk(int, int, World*);
    ~Chunk();

    void generateMesh();
    void generateMesh(std::vector<unsigned short>);
    void reloadMesh(unsigned, unsigned, unsigned);
    void draw(unsigned);

    unsigned short getBlock(unsigned, unsigned, unsigned);
    unsigned short getBlockGlobal(long, long, long);
    bool removeBlock(unsigned, unsigned, unsigned);
    bool removeBlockMesh(unsigned, unsigned, unsigned);
    void clearMesh();
    glm::uvec3 getPos();

private:
    void generateTerrain();
    void renderInit();
    void loadFace(const MeshData*, unsigned , unsigned, unsigned, unsigned);

    std::array<unsigned short, 4>
    getOcclusion(unsigned, unsigned, unsigned, unsigned short);
    std::vector<unsigned short> blockArray;
    glm::uvec3 pos;

    World* world;

    unsigned VAO;
    unsigned VBO;
    unsigned EBO;
    unsigned indexSize;

    std::vector<GLuint> vertexMesh;
    std::vector<unsigned> indexMesh;
};
