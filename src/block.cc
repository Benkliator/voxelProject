#include "block.h"

#include <sys/types.h>

// NOTE: Additional bits might be block unique, consider for future.
Block::Block(ushort blockType) {
    switch (blockType) {
    case Block::Air: {
        // Skipped in drawing
    } break;

    case Block::Grass: {
        top = 0b00000000;
        bottom = 0b00100000;
        side = 0b00010000;
    } break;

    case Block::Dirt: {
        top = 0b00100000;
        bottom = 0b00100000;
        side = 0b00100000;
    } break;

    case Block::Stone: {
        top = 0b00110000;
        bottom = 0b00110000;
        side = 0b00110000;
    } break;

    case Block::Water: {
        top = 0b00000011;
        bottom = 0b00000011;
        side = 0b00000011;
    } break;
    }
}

bool isAir(unsigned block) {
    return (block & typeMask) == Block::Air;
}
