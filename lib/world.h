#pragma once

#include "chunk.h"

#include <functional>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <vector>

const unsigned worldHeight = 254;
//////////////////////////////////////////////////////////////////

class Chunk;

// TODO: Modify visible chunks and adjust what is being rendered
// based on camera/player position.
class World {
public:
    World(unsigned, unsigned);
    ~World();

    // Draws out every chunk in visibleChunks
    void draw(glm::mat4& view);
    std::optional<ushort> getBlock(long, long, long);
    std::optional<std::reference_wrapper<Chunk>>
    getChunk(unsigned, unsigned, unsigned);
    void reloadChunksAround(unsigned, unsigned, unsigned);

private:
    void shaderInit();

    std::vector<Chunk> visibleChunks;
    unsigned renderDistance;
    unsigned offset;

    unsigned textureMap;
    unsigned shaderProgram;
    glm::mat4 projection;
};
