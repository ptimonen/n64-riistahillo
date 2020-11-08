#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "math_util.h"

#define CHAIN_MAX_NODE_COUNT 8
#define PLAYER_CHAIN_SEGMENT_LENGTH 200

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
} Player;

static inline VerletBody* player_getCharacter(Player* player) {
    return &player->chain.nodes[0];
}

static inline VerletBody* player_getBoulder(Player* player) {
    return &player->chain.nodes[player->chain.nodeCount - 1];
}

typedef struct Camera {
    Vec3f position;
    Vec3f target;
    Vec3f up;
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
    Player player;
    Camera camera;
    Physics physics;
} GameState;

#endif