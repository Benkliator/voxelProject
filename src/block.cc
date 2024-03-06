#include "../lib/block.h"

// TODO: Change this (maybe?)
//
// NOTE: Additional bits might be block unique, consider for future.
Block::Block(unsigned short blockType)
{
    switch(blockType){
        // Grass
        case 0:
            top     = 0;
            bottom  = 32;
            side    = 16;
        break;

        // Dirt
        case 1:
            top     = 32;
            bottom  = 32;
            side    = 32;
        break;

        // Stone
        case 2:
            top     = 48;
            bottom  = 48;
            side    = 48;
        break;

        // Purple
        case 3:
            top     = 0b01000010;
            bottom  = 0b01000010;
            side    = 0b01000010;
        break;

        // Red
        case 4:
            top     = 0b00100010;
            bottom  = 0b00100010;
            side    = 0b00100010;
        break;
    }
}
