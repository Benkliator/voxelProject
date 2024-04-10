#define MAIN

#include "game.h"
#include <memory>

int main(void) {
    std::unique_ptr<Game> game = std::make_unique<Game>();
    game->gameLoop();
    // This delete causes seg fault?
    // delete game;
}
