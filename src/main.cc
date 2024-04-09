#define MAIN

#include "game.h"

int main(void) {
    Game* game = new Game{};
    game->gameLoop();
    // This delete causes seg fault?
    // delete game;
}
