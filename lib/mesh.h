#pragma once

#include "block.h"

struct MeshData {
    const unsigned faceCoords[12];
    const unsigned texCoords[8];
    ushort blockFace;
};

const MeshData topMeshData = {
    // Vertex Coordinates
    {
        1, 1, 0,
        1, 1, 1,
        0, 1, 1,
        0, 1, 0,
    },
    // Texture Coordinates
    {
        1, 0,
        1, 1,
        0, 1,
        0, 0,
    },
    // What type is block face
    Block::Top
};

const MeshData bottomMeshData = {
    // Vertex Coordinates
    {
        0, 0, 0,
        0, 0, 1,
        1, 0, 1,
        1, 0, 0,
    },
    // Texture Coordinates
    {
        0, 0,
        0, 1,
        1, 1,
        1, 0,
    },
    // What type is block face
    Block::Bottom
};

const MeshData backMeshData = {
    // Vertex Coordinates
    {
        1, 0, 0,
        1, 1, 0,
        0, 1, 0,
        0, 0, 0,
    },
    // Texture Coordinates
    {
        0, 1,
        0, 0,
        1, 0,
        1, 1,
    },
    // What type is block face
    Block::Back
};

const MeshData frontMeshData = {
    // Vertex Coordinates
    {
        1, 1, 1,
        1, 0, 1,
        0, 0, 1,
        0, 1, 1,
    },
    // Texture Coordinates
    {
        0, 0,
        0, 1,
        1, 1,
        1, 0,
    },
    // What type is block face
    Block::Front
};

const MeshData leftMeshData = {
    // Vertex Coordinates
    {
        0, 1, 1,
        0, 0, 1,
        0, 0, 0,
        0, 1, 0,
    },
    // Texture Coordinates
    {
        1, 0,
        1, 1,
        0, 1,
        0, 0,
    },
    // What type is block face
    Block::Left
};

const MeshData rightMeshData = {
    // Vertex Coordinates
    {
        1, 1, 0,
        1, 0, 0,
        1, 0, 1,
        1, 1, 1,
    },
    // Texture Coordinates
    {
        0, 0,
        0, 1,
        1, 1,
        1, 0,
    },
    // What type is block face
    Block::Right
};
