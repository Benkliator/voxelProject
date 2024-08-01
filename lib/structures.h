#pragma once

#include <glm/vec3.hpp>

#include "block.h"
#include "vector"

// NOTE: 
// This is built up of structs instead of raw vectors,
// this is because future development may want to benefit
// from structs.

struct StructureData {
    const std::vector<std::pair<glm::ivec3, enum Block::BlockType>> data;
};

const StructureData treeLeaves = {
    {
        {glm::ivec3(-2, 0, -1), Block::Leaf},
        {glm::ivec3(-2, 0, 0), Block::Leaf},
        {glm::ivec3(-2, 0, 1), Block::Leaf},

        {glm::ivec3(-1, 0, -2), Block::Leaf},
        {glm::ivec3(-1, 0, -1), Block::Leaf},
        {glm::ivec3(-1, 0, 0), Block::Leaf},
        {glm::ivec3(-1, 0, 1), Block::Leaf},
        {glm::ivec3(-1, 0, 2), Block::Leaf},

        {glm::ivec3(0, 0, -2), Block::Leaf},
        {glm::ivec3(0, 0, -1), Block::Leaf},
        //{glm::ivec3(0, 0, 0), Block::Leaf},
        {glm::ivec3(0, 0, 1), Block::Leaf},
        {glm::ivec3(0, 0, 2), Block::Leaf},

        {glm::ivec3(1, 0, -2), Block::Leaf},
        {glm::ivec3(1, 0, -1), Block::Leaf},
        {glm::ivec3(1, 0, 0), Block::Leaf},
        {glm::ivec3(1, 0, 1), Block::Leaf},
        {glm::ivec3(1, 0, 2), Block::Leaf},
        
        {glm::ivec3(2, 0, -1), Block::Leaf},
        {glm::ivec3(2, 0, 0), Block::Leaf},
        {glm::ivec3(2, 0, 1), Block::Leaf},

        {glm::ivec3(-2, 1, -1), Block::Leaf},
        {glm::ivec3(-2, 1, 0), Block::Leaf},
        {glm::ivec3(-2, 1, 1), Block::Leaf},

        {glm::ivec3(-1, 1, -2), Block::Leaf},
        {glm::ivec3(-1, 1, -1), Block::Leaf},
        {glm::ivec3(-1, 1, 0), Block::Leaf},
        {glm::ivec3(-1, 1, 1), Block::Leaf},
        {glm::ivec3(-1, 1, 2), Block::Leaf},

        {glm::ivec3(0, 1, -2), Block::Leaf},
        {glm::ivec3(0, 1, -1), Block::Leaf},
        {glm::ivec3(0, 1, 0), Block::Leaf},
        {glm::ivec3(0, 1, 1), Block::Leaf},
        {glm::ivec3(0, 1, 2), Block::Leaf},

        {glm::ivec3(1, 1, -2), Block::Leaf},
        {glm::ivec3(1, 1, -1), Block::Leaf},
        {glm::ivec3(1, 1, 0), Block::Leaf},
        {glm::ivec3(1, 1, 1), Block::Leaf},
        {glm::ivec3(1, 1, 2), Block::Leaf},
        
        {glm::ivec3(2, 1, -1), Block::Leaf},
        {glm::ivec3(2, 1, 0), Block::Leaf},
        {glm::ivec3(2, 1, 1), Block::Leaf},

        {glm::ivec3(-1, 2, -1), Block::Leaf},
        {glm::ivec3(-1, 2, 0), Block::Leaf},
        {glm::ivec3(-1, 2, 1), Block::Leaf},

        {glm::ivec3(0, 2, -1), Block::Leaf},
        {glm::ivec3(0, 2, 0), Block::Leaf},
        {glm::ivec3(0, 2, 1), Block::Leaf},

        {glm::ivec3(1, 2, -1), Block::Leaf},
        {glm::ivec3(1, 2, 0), Block::Leaf},
        {glm::ivec3(1, 2, 1), Block::Leaf},

        {glm::ivec3(-1, 3, 0), Block::Leaf},

        {glm::ivec3(0, 3, -1), Block::Leaf},
        {glm::ivec3(0, 3, 0), Block::Leaf},
        {glm::ivec3(0, 3, 1), Block::Leaf},

        {glm::ivec3(1, 3, 0), Block::Leaf},
    }
};
