#include "game_setup.h"
#include "game_state.h"
#include "globals.h"

void setupChain(Chain* chain, Vec2f position) {
    int i;
    chain->nodeCount = CHAIN_MAX_NODE_COUNT;
    chain->segmentLength = SMALL_CHAIN_SEGMENT_LENGTH;

    for(i = 0; i < chain->nodeCount; ++i) {
        chain->nodes[i].position = add2f(position, (Vec2f){0, -i * chain->segmentLength});
        chain->nodes[i].oldPosition = chain->nodes[i].position;
        chain->nodes[i].radius = 0;
        chain->nodes[i].bounciness = 0.8f;
        chain->nodes[i].mass = 1.0f;
        chain->nodes[i].airResistance = 0.9999f;
        chain->nodes[i].collisionMassMultiplier = 1.0f;
    }

    chain->nodes[0].radius = 100.0f;
    chain->nodes[0].mass = 100.0f;
    chain->nodes[chain->nodeCount - 1].radius = SMALL_BOULDER_RADIUS;
    chain->nodes[chain->nodeCount - 1].mass = 10.0f;
    chain->nodes[chain->nodeCount - 1].collisionMassMultiplier = 10.0f;
}

void setupPlayer(Player* player, int index) {
    static const Vec2f startPositions[MAX_PLAYERS] = {
        {-1000, +700},
        {+1000, +700},
        {-800, -300},
        {+800, -300},
    };
    player->index = index;
    player->movementSpeed = 120.0f;
    player->movementControl = (Vec2f){0, 0};
    player->score = 0;
    player->invulnerabilityTimer = 0.0f;
    player->bigBoulderTimer = 0.0f;
    setupChain(&player->chain, startPositions[index]);
}

void setupEnemy(Enemy* enemy) {
    enemy->despawnTimer = 0.0f;
    enemy->health = 0;
    enemy->verletBody.position = (Vec2f){0, 0};
    enemy->verletBody.oldPosition = (Vec2f){0, 0};
    enemy->verletBody.mass = 10.0f;
    enemy->verletBody.bounciness = 0.5;
    enemy->verletBody.airResistance = 1.0f;
}

void setupCamera(Camera* camera) {
    camera->startPosition = (Vec3f){0.0f, 0.0f, 2400.0f};
    camera->position = camera->startPosition;
    camera->target = (Vec3f){0.0f, 0.0f, 0.0f};
    camera->up = (Vec3f){0.0f, 1.0f, 0.0f};
}

void setupPhysics(Physics* physics) {
    physics->verletConstraintIterations = 16;
    physics->timeAtLastPhysicsUpdate = 0.0f;
    physics->deltaTime = 0.0f;
    physics->gravity = (Vec2f){0, -10};
    physics->minBoundary = (Vec2f){-1400, -1100};
    physics->maxBoundary = (Vec2f){1400, 1100};
}

void setupGameState(GameState* gameState, GameConfig* gameConfig) {
    static float enemySpawnIntervalForPlayerCount[] = {3.1f, 3.0f, 2.9f, 2.9f};
    int i;
    gameState->hideMeshes = 0;
    gameState->nextEnemyTargetPlayerIndex = 0;
    gameState->spawnsUntilNextBigEnemy = BIG_ENEMY_SPAWN_INTERVAL;
    gameState->enemySpawnInterval = enemySpawnIntervalForPlayerCount[gameConfig->playerCount - 1];
    gameState->enemySpawnTimer = gameState->enemySpawnInterval;
    gameState->endTimer = 5.0f;
    for(i = 0; i < MAX_PLAYERS; ++i) {
        setupPlayer(&gameState->players[i], i);
        if((gameConfig->playerCount - 1) >= i ) {
            gameState->players[i].health = 3;
            gameState->players[i].despawnTimer = 3.0f;
        }
        else {
            gameState->players[i].health = 0;
            gameState->players[i].despawnTimer = 0.0f;
        }
    }
    for(i = 0; i < MAX_ENEMIES; ++i) {
        setupEnemy(&gameState->enemies[i]);
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
    programState->gameState.deltaTime = 0.0f;
    programState->gameState.lastTick = 0.0f;
}