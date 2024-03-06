#pragma once
#include <vector>


// Im unsure of this class implementation, raw structs for this
// might be better.
class Block
{
    public:
        Block(unsigned short blockType);

        ~Block() = default;


        unsigned short top;
        unsigned short bottom;
        unsigned short side;
};
