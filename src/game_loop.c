#include "game_loop.h"

#include "audio.h"
#include "globals.h"
#include "render.h"
#include "input.h"
#include "physics.h"
#include "game_setup.h"

void checkEndCondition(ProgramState* programState) {
    GameConfig* gameConfig = &programState->gameConfig;
    GameState* gameState = &programState->gameState;
    int alivePlayers = 0;
    int i;
    for(i = 0; i < MAX_PLAYERS; ++i) {
        alivePlayers += gameState->players[i].health > 0;
    }
    {
        int gameOver = (gameConfig->gameMode == BATTLE && alivePlayers < 2) ||
                       (gameConfig->gameMode == SURVIVAL && alivePlayers == 0);
        if (gameOver) {
            gameState->endTimer = MAX(0.0f, gameState->endTimer - gameState->physics.deltaTime);
        }
        if (gameState->endTimer <= 0.0f) {
            programState->activeScreen = END;
        }
    }
}

void gameLoop() {
    ProgramState* programState = &g_programState;

    if(programState->gameState.freezeFrame > 0) {
        --programState->gameState.freezeFrame;
        return;
    }

    updatePhysics(&programState->gameState, programState->gameConfig.gameMode);
    updateAudio(programState->gameState.physics.deltaTime);

    if(programState->activeScreen == MENU) {
        updateMenuInput(programState);
    }
    if(programState->activeScreen == LOADING) {
        setupGameState(&programState->gameState, &programState->gameConfig);
        programState->activeScreen = GAME;
    }
    if(programState->activeScreen == GAME) {
        updateGameInput(programState);
        checkEndCondition(programState);
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
