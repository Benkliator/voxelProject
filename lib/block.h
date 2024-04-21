#pragma once

#include <sys/types.h>

const ushort xMask = 0b00000000000000000001111;
const ushort yMask = 0b00000001111111100000000;
const ushort zMask = 0b00000000000000011110000;
const ushort typeMask = 0b1111111100000000;

const ushort xOffset = __builtin_ctz(xMask);
const ushort yOffset = __builtin_ctz(yMask);
const ushort zOffset = __builtin_ctz(zMask);
const ushort typeOffset = __builtin_ctz(typeMask);

const ushort topMask = 0b10000000;
const ushort bottomMask = 0b01000000;
const ushort backMask = 0b00100000;
const ushort frontMask = 0b00010000;
const ushort leftMask = 0b00001000;
const ushort rightMask = 0b00000100;

// Im unsure of this class implementation, raw structs for this
// might be better.
struct Block {
    Block(ushort);
    ~Block() = default;

    enum BlockType : ushort {
        Air,
        Dirt,
        Grass,
        Stone,
        Water,
    };

    enum BlockFace : ushort {
        Top,
        Bottom,
        Front,
        Back,
        Left,
        Right,
    };

    ushort top;
    ushort bottom;
    ushort side;
};

// Helper functions
bool isAir(unsigned);
