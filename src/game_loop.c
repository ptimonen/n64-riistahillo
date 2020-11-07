#include "game_loop.h"

#include <ultra64.h>

#include "globals.h"
#include "render.h"
#include "input.h"
#include "physics.h"

float updateDeltaTime(GameState* gameState) {
    const float c_maxDeltaTime = 1.0f / 30.0f;
    float timeNow = OS_CYCLES_TO_NSEC(osGetTime()) / 1e-9f;
    gameState->deltaTime = MIN(c_maxDeltaTime, timeNow - gameState->timeAtGameLoopStart);
    gameState->timeAtGameLoopStart = timeNow;
}

void gameLoop() {
    updateDeltaTime(&g_gameState);
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
