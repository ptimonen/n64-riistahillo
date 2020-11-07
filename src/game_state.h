#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "math_util.h"

#define PLAYER_CHAIN_NODE_COUNT 8
#define PLAYER_CHAIN_SEGMENT_LENGTH 200

typedef struct ChainNode {
    Vec3f position;
    Vec3f oldPosition;
} ChainNode;

typedef struct Character {
    float movementSpeed;
    Vec3f position;
    Vec3f velocity;
} Character;

typedef struct Player {
    ChainNode chainNodes[PLAYER_CHAIN_NODE_COUNT];
    Character character;
} Player;

typedef struct Camera {
    Vec3f position;
    Vec3f target;
    Vec3f up;
} Camera;

typedef struct GameState {
    float timeAtGameLoopStart;
    float deltaTime;
    int hideMeshes;
    Vec3f gravity;
    Player player;
    Camera camera;
} GameState;

#endif