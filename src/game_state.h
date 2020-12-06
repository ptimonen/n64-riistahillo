#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "math_util.h"

#define CHAIN_MAX_NODE_COUNT 8
#define PLAYER_CHAIN_SEGMENT_LENGTH 150

typedef struct VerletBody {
    Vec2f position;
    Vec2f oldPosition;
    float mass;
    float radius;
    float bounciness;
    int isStatic;
} VerletBody;

typedef struct ChainNode {
    Vec2f position;
    Vec2f oldPosition;
} ChainNode;

typedef struct Chain {
    VerletBody nodes[CHAIN_MAX_NODE_COUNT];
    int nodeCount;
    int segmentLength;
} Chain;

typedef struct Player {
    Chain chain; // First and last links correspond to player and boulder
    float movementSpeed;
    Vec2f movementControl;
    int health;
    int index;
    int score;
} Player;

static inline VerletBody* player_getCharacter(Player* player) {
    return &player->chain.nodes[0];
}

static inline VerletBody* player_getBoulder(Player* player) {
    return &player->chain.nodes[player->chain.nodeCount - 1];
}

typedef struct Camera {
    Vec3f startPosition;
    Vec3f position;
    Vec3f target;
    Vec3f up;
    float screenShake;
} Camera;

typedef struct Physics {
    int verletConstraintIterations;
    float timeAtLastPhysicsUpdate;
    float deltaTime;
    Vec2f gravity;
    Vec2f minBoundary;
    Vec2f maxBoundary;
} Physics;

typedef struct GameState {
    int hideMeshes;
    Player players[4];
    Camera camera;
    Physics physics;
} GameState;

typedef enum GameMode { SURVIVAL, BATTLE } GameMode;

typedef struct GameConfig {
    int playerCount;
    GameMode gameMode;
} GameConfig;

typedef enum ScreenType { MENU, LOADING, GAME, END } ScreenType;

typedef struct ProgramState {
    ScreenType activeScreen;
    GameConfig gameConfig;
    GameState gameState;
} ProgramState;

#endif