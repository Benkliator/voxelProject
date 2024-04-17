#pragma once

////////////Important values when it comes to bitwise operations//
const unsigned xMask = 0b00000000000000000001111;
const unsigned yMask = 0b00000001111111100000000;
const unsigned zMask = 0b00000000000000011110000;
const unsigned typeMask = 0b11110000000000000000000;

const unsigned xOffset = __builtin_ctz(xMask);
const unsigned yOffset = __builtin_ctz(yMask);
const unsigned zOffset = __builtin_ctz(zMask);
const unsigned blockTypeOffset = __builtin_ctz(typeMask);

// Im unsure of this class implementation, raw structs for this
// might be better.
class Block {
public:
    enum BlockType : unsigned short {
        Air,
        Dirt,
        Grass,
        Stone,
    };

    enum BlockFace : unsigned short {
        Top,
        Bottom,
        Front,
        Back,
        Left,
        Right,
    };

    Block(unsigned);

    ~Block() = default;

    unsigned short top;
    unsigned short bottom;
    unsigned short side;
};

// Helper functions
bool isAir(unsigned);
