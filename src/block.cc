#include "block.h"

#include <sys/types.h>

// NOTE: Additional bits might be block unique, consider for future.
Block::Block(ushort blockType) {
    switch (blockType) {
    case Block::Air: {
        // Skipped in drawing
    } break;

    case Block::Grass: {
        top     = 0b00000000;
        bottom  = 0b00100000;
        side    = 0b00010000;
    } break;

    case Block::FlowerGrass: {
        top     = 0b00100010;
        bottom  = 0b00100000;
        side    = 0b00010000;
    } break;

    case Block::Dirt: {
        top     = 0b00100000;
        bottom  = 0b00100000;
        side    = 0b00100000;
    } break;

    case Block::Stone: {
        top     = 0b00110000;
        bottom  = 0b00110000;
        side    = 0b00110000;
    } break;

    case Block::Water: {
        top     = 0b00000011;
        bottom  = 0b00000011;
        side    = 0b00000011;
    } break;

    case Block::Plank: {
        top     = 0b00100001;
        bottom  = 0b00100001;
        side    = 0b00100001;
    } break;

    case Block::Log: {
        top     = 0b00010001;
        bottom  = 0b00010001;
        side    = 0b00000001;
    } break;

    case Block::Wall: {
        top     = 0b00110001;
        bottom  = 0b00110001;
        side    = 0b00110001;
    } break;

    case Block::BrokenWall: {
        top     = 0b01000010;
        bottom  = 0b01000010;
        side    = 0b01000010;
    } break;

    case Block::FlowerWall: {
        top     = 0b00110010;
        bottom  = 0b00110010;
        side    = 0b00110010;
    } break;

    case Block::Brick: {
        top     = 0b01000001;
        bottom  = 0b01000001;
        side    = 0b01000001;
    } break;
    
    case Block::Sand: {
        top     = 0b01000000;
        bottom  = 0b01000000;
        side    = 0b01000000;
    } break;
    }
}

std::string blockToString(Block::BlockType bt) {
    switch (bt) {
    case Block::Air:
        return "Air";
    case Block::Dirt:
        return "Dirt";
    case Block::Grass:
        return "Grass";
    case Block::FlowerGrass:
        return "FlowerGrass";
    case Block::Stone:
        return "Stone";
    case Block::Water:
        return "Water";
    case Block::Plank:
        return "Plank";
    case Block::Log:
        return "Log";
    case Block::Wall:
        return "Wall";
    case Block::BrokenWall:
        return "BrokenWall";
    case Block::FlowerWall:
        return "FlowerWall";
    case Block::Brick:
        return "Brick";
    case Block::Sand:
        return "Sand";
    case Block::NUM_BLOCKTYPES:
        break;
    }
    __builtin_unreachable();
};

bool isAir(unsigned block) {
    return (block & typeMask) == Block::Air;
}
