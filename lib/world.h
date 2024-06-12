#pragma once

#include "chunk.h"
#include "synchQueue.h"

#include <functional>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <vector>
#include <thread>
#include <chrono>
#include <deque>

const unsigned worldHeight = 254;
//////////////////////////////////////////////////////////////////

class Chunk;

// TODO: Modify visible chunks and adjust what is being rendered
// based on camera/player position.
class World {
public:
    World(unsigned, glm::vec3);
    ~World();

    // Draws out every chunk in visibleChunks
    void draw(glm::mat4& view);
    std::optional<ushort> getBlock(long, long, long);
    std::optional<std::reference_wrapper<Chunk>>
    getChunkSlow(unsigned, unsigned, unsigned);
    std::optional<std::reference_wrapper<Chunk>>
    getChunkFast(unsigned, unsigned, unsigned);

    void reloadChunksAround(unsigned, unsigned, unsigned);
    bool meshCatchup();
    void fullMeshCatchup();

private:
    void shaderInit();

    std::vector<Chunk> visibleChunks;
    //synchQueue<Chunk*> loadQueue;
    //std::queue<Chunk*> loadQueue;

    std::deque<Chunk*> loadQueue;

    bool meshLoad = true;

    glm::uvec3 worldCenter;
    unsigned renderDistance;

    unsigned textureMap;
    unsigned shaderProgram;
    glm::mat4 projection;
};
