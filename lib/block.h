#pragma once

// Im unsure of this class implementation, raw structs for this
// might be better.
class Block {
public:
    enum BlockType : int {
        Air = 0,
        Dirt,
        Grass,
        Stone,
    };

    Block(unsigned short blockType);

    ~Block() = default;

    unsigned short top;
    unsigned short bottom;
    unsigned short side;
};
