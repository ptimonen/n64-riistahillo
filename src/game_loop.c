#include "game_loop.h"

#include "globals.h"
#include "render.h"
#include "input.h"
#include "physics.h"
#include "game_setup.h"

void gameLoop() {
    ProgramState* programState = &g_programState;
    if(programState->activeScreen == MENU) {
        updateMenuInput(programState);
    }
    if(programState->activeScreen == LOADING) {
        setupGameState(&programState->gameState, &programState->gameConfig);
        programState->activeScreen = GAME;
    }
    if(programState->activeScreen == GAME) {
        updateGameInput(programState);
        updatePhysics(&programState->gameState);
    }
    if(programState->activeScreen == END) {
        updateEndInput(programState);
    }
}

void tick(int pendingGfx) {
    // Single-buffered rendering.
    // TODO: could also use double-buffering here, see: http://n64devkit.square7.ch/nusystem/tutorial/4.htm
    if (pendingGfx < 1) {
        render(&g_programState);
    }

    gameLoop();
}
