#include "game_setup.h"
#include "game_state.h"
#include "globals.h"

void setupChain(Chain* chain) {
    int i;
    chain->nodeCount = CHAIN_MAX_NODE_COUNT;
    chain->segmentLength = 70;

    for(i = 0; i < chain->nodeCount; ++i) {
        chain->nodes[i].position = (Vec2f){0, -i * chain->segmentLength};
        chain->nodes[i].oldPosition = chain->nodes[i].position;
        chain->nodes[i].radius = 0;
        chain->nodes[i].bounciness = 0.5f;
        chain->nodes[i].mass = 1.0f;
        chain->nodes[i].isStatic = FALSE;
    }

    chain->nodes[0].radius = 100.0f;
    chain->nodes[0].mass = 100.0f;
    chain->nodes[chain->nodeCount - 1].radius = 50.0f;
    chain->nodes[chain->nodeCount - 1].mass = 20.0f;
}

void setupPlayer(Player* player) {
    player->movementSpeed = 20.0f;
    player->movementControl = (Vec2f){0, 0};
    setupChain(&player->chain);
}

void setupCamera(Camera* camera) {
    camera->position = (Vec3f){0.0f, 0.0f, 2400.0f};
    camera->target = (Vec3f){0.0f, 0.0f, 0.0f};
    camera->up = (Vec3f){0.0f, 1.0f, 0.0f};
}

void setupPhysics(Physics* physics) {
    physics->verletConstraintIterations = 32;
    physics->timeAtLastPhysicsUpdate = 0.0f;
    physics->deltaTime = 0.0f;
    physics->gravity = (Vec2f){0, -10};
    physics->minBoundary = (Vec2f){-1200, -900};
    physics->maxBoundary = (Vec2f){1200, 900};
}

void setupGameState(GameState* gameState, GameConfig* gameConfig) {
    gameState->hideMeshes = 0;
    for(i = 0; i < 4; ++i) {
        setupPlayer(&gameState->players[i]);
        if((gameConfig->playerCount - 1) >= i ) {
            gameState->players[i].health = 3;
        }
        else {
            gameState->players[i].health = 0;
        }
        // TODO: Set proper start locations
    }
    setupCamera(&gameState->camera);
    setupPhysics(&gameState->physics);
}

void setupGameConfig(GameConfig* gameConfig) {
    gameConfig->playerCount = 1;
    gameConfig->gameMode = SURVIVAL;
}

void setupProgramState(ProgramState* programState) {
    programState->activeScreen = MENU;
    setupGameConfig(&programState->gameConfig);
    setupGameState(&programState->gameState, &programState->gameConfig);
}