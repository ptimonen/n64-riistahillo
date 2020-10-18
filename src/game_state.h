#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "math.h"

typedef struct Player {
    float movementSpeed;
    Vec3d position;
    Vec3d velocity;
} Player;

typedef struct GameState {
    float deltaTime;
    int hideMeshes;
    Vec3d gravity;
    Player player;
} GameState;

#endif