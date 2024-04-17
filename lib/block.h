#pragma once

// Im unsure of this class implementation, raw structs for this
// might be better.
class Block {
public:
    enum BlockType : unsigned short {
        Air = 0,
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

    Block(unsigned short blockType);

    ~Block() = default;

    unsigned short top;
    unsigned short bottom;
    unsigned short side;
};
