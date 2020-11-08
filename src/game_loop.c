#include "game_loop.h"

#include "globals.h"
#include "render.h"
#include "input.h"
#include "physics.h"

void gameLoop() {
    updateInput(&g_gameState);
    updatePhysics(&g_gameState);
}

void tick(int pendingGfx) {
    // Single-buffered rendering.
    // TODO: could also use double-buffering here, see: http://n64devkit.square7.ch/nusystem/tutorial/4.htm
    if (pendingGfx < 1) {
        render(&g_gameState);
    }

    gameLoop();
}
