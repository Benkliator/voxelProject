#include "block.h"

// TODO: Change this (maybe?)
//
// NOTE: Additional bits might be block unique, consider for future.
Block::Block(unsigned short blockType) {
    switch (blockType) {
    // Grass
    case Block::Air:
        // Skipped in drawing
        break;

    case Block::Grass:
        top = 0;
        bottom = 0b00100000;
        side = 0b00010000;
        break;

    // Dirt
    case Block::Dirt:
        top = 0b00100000;
        bottom = 0b00100000;
        side = 0b00100000;
        break;

    // Stone
    case Block::Stone:
        top = 0b00110000;
        bottom = 0b00110000;
        side = 0b00110000;
        break;
    }
}
