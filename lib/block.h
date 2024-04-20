#pragma once

#include <cstdint>

const unsigned xMask = 0b00000000000000000001111;
const unsigned yMask = 0b00000001111111100000000;
const unsigned zMask = 0b00000000000000011110000;
const unsigned typeMask = 0b1111111100000000;

const unsigned xOffset = __builtin_ctz(xMask);
const unsigned yOffset = __builtin_ctz(yMask);
const unsigned zOffset = __builtin_ctz(zMask);
const unsigned typeOffset = __builtin_ctz(typeMask);

// TODO(Christoffer): Better error block for visually showing it?
//                    currently only used for returning values for blocks
//                    that are requested outside of the loaded chunk bounds
const unsigned errorBlock = UINT16_MAX;

// Im unsure of this class implementation, raw structs for this
// might be better.
struct Block {
    Block(unsigned);
    ~Block() = default;

    enum BlockType : unsigned short {
        Air,
        Dirt,
        Grass,
        Stone,
        Water,
    };

    enum BlockFace : unsigned short {
        Top,
        Bottom,
        Front,
        Back,
        Left,
        Right,
    };

    unsigned short top;
    unsigned short bottom;
    unsigned short side;
};

// Helper functions
bool isAir(unsigned);
