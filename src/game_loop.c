#include "game_loop.h"

#include <stdlib.h>
#include <math.h>

#include "audio.h"
#include "globals.h"
#include "render.h"
#include "input.h"
#include "physics.h"
#include "game_setup.h"

#include <ultra64.h>

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
            sndHandle = nuAuStlSndPlayerPlay(SND_GAMEEND);
            programState->activeScreen = END;
        }
    }
}

void trySpawnEnemy(GameState* gameState) {
    int enemyIndex = 0;
    if (gameState->enemySpawnTimer > 0.0f) {
        gameState->enemySpawnTimer -= gameState->physics.deltaTime;
        return;
    }

    while(enemy_exists(&gameState->enemies[enemyIndex])) {
        ++enemyIndex;
        if(enemyIndex >= MAX_ENEMIES) {
            return;
        }
    }

    gameState->enemySpawnTimer = gameState->enemySpawnInterval;
    if(gameState->enemySpawnInterval > 2.5f) {
        gameState->enemySpawnInterval -= 0.1f;
    }
    else if(gameState->enemySpawnInterval > 2.0f) {
        gameState->enemySpawnInterval -= 0.075f;
    }
    else if(gameState->enemySpawnInterval > 1.0f) {
        gameState->enemySpawnInterval -= 0.05f;
    }
    else if(gameState->enemySpawnInterval > 0.75f) {
        gameState->enemySpawnInterval -= 0.01f;
    }

    if(gameState->enemySpawnInterval < 0.3f) {
        gameState->enemySpawnInterval = 0.3f;
    }

    // Set enemy target round robin.
    {
        Enemy* enemy = &gameState->enemies[enemyIndex];
        int i;
        for (i = 0; i < MAX_PLAYERS; ++i) {
            int playerIndex = (gameState->nextEnemyTargetPlayerIndex + i) % MAX_PLAYERS;
            if (gameState->players[playerIndex].health > 0) {
                float random = rand() / (float)RAND_MAX;
                Vec2f direction = (Vec2f){cosf(2.0f * M_PI * random), sinf(2.0f * M_PI * random)};
                Vec2f position = mul2f(direction, 1500.0f);
                enemy->health = 1;
                enemy->despawnTimer = 3.0f;
                enemy->targetPlayerIndex = playerIndex;
                enemy->verletBody.position = position;
                enemy->verletBody.oldPosition = position;
                enemy->bobTimer = 0.0f;
                if(gameState->spawnsUntilNextBigEnemy == 0) {
                    enemy->verletBody.mass = 100;
                    enemy->verletBody.radius = 200;
                    enemy->movementSpeed = 200.0f;
                    enemy->isBig = TRUE;
                    gameState->spawnsUntilNextBigEnemy = BIG_ENEMY_SPAWN_INTERVAL;
                    sndHandle = nuAuStlSndPlayerPlay(SND_MODESET);
                } else {
                    enemy->verletBody.mass = 10;
                    if(gameState->enemySpawnInterval > 1.5f) {
                        enemy->verletBody.radius = 130;
                        enemy->movementSpeed = 150.0f;
                    }
                    else if(gameState->enemySpawnInterval > 1.0f) {
                        enemy->verletBody.radius = 120;
                        enemy->movementSpeed = 200.0f;
                    }
                    else {
                        enemy->verletBody.radius = 110;
                        enemy->movementSpeed = 250.0f;
                    }

                    enemy->isBig = FALSE;
                    --gameState->spawnsUntilNextBigEnemy;
                }
                enemy->rotation = 0.0f;
                enemy->rotationSpeed = 0.0f;
                gameState->nextEnemyTargetPlayerIndex = (playerIndex + 1) % MAX_PLAYERS;
                return;
            }
        }
    }
}

void gameLoop() {
    ProgramState* programState = &g_programState;

    float timeNow = OS_CYCLES_TO_NSEC(osGetTime()) / 1e9f;
    programState->gameState.deltaTime = timeNow - programState->gameState.lastTick;
    programState->gameState.lastTick = timeNow;

    updateAudio(programState->gameState.deltaTime);

    if(programState->gameState.freezeFrame > 0) {
        --programState->gameState.freezeFrame;
    }
    else if(programState->activeScreen == MENU) {
        updateMenuInput(programState);
    }
    else if(programState->activeScreen == LOADING) {
        setupGameState(&programState->gameState, &programState->gameConfig);
        programState->activeScreen = GAME;
    }
    else if(programState->activeScreen == GAME) {
        if(programState->gameConfig.gameMode == SURVIVAL) {
            trySpawnEnemy(&programState->gameState);
        }
        updateGameInput(programState);
        updatePhysics(&programState->gameState, programState->gameConfig.gameMode);
        checkEndCondition(programState);
    }
    else if(programState->activeScreen == END) {
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
