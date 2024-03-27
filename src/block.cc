#include "block.h"

// TODO: Change this (maybe?)
//
// NOTE: Additional bits might be block unique, consider for future.
Block::Block(unsigned short blockType) {
    switch (blockType) {
    // Grass
    case 0:
        top = 0;
        bottom = 0b00100000;
        side = 0b00010000;
        break;

    // Dirt
    case 1:
        top = 0b00100000;
        bottom = 0b00100000;
        side = 0b00100000;
        break;

    // Stone
    case 2:
        top = 0b00110000;
        bottom = 0b00110000;
        side = 0b00110000;
        break;

    // Purple
    case 3:
        top = 0b01000010;
        bottom = 0b01000010;
        side = 0b01000010;
        break;

    // Red
    case 4:
        top = 0b00100010;
        bottom = 0b00100010;
        side = 0b00100010;
        break;
    }
}
