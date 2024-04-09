#define MAIN

#include "game.h"

int main(void) {
    Game* game = new Game{};
    game->gameLoop();
}
