#include "game_setup.h"

#include "game_state.h"

void setupCharacter(Character* character) {
    character->movementSpeed = 200;
    character->position = (Vec3d){0, 0, 0};
    character->velocity = (Vec3d){0, 0, 0};
}

void setupPlayer(Player* player) {
    int i;

    for (i = 0; i < PLAYER_CHAIN_NODE_COUNT; ++i) {
        Vec3d segment = (Vec3d){0, PLAYER_CHAIN_SEGMENT_LENGTH, 0};
        Vec3d position = add(player->character.position, mul(segment, i));

        player->chainNodes[i].position = position;
        player->chainNodes[i].oldPosition = position;
    }

    setupCharacter(&player->character);
}

void setupCamera(Camera* camera) {
    camera->position = (Vec3d){0.0f, 0.0f, 2400.0f};
    camera->target = (Vec3d){0.0f, 0.0f, 0.0f};
    camera->up = (Vec3d){0.0f, 1.0f, 0.0f};
}

void setupGameState(GameState* gameState) {
    gameState->timeAtGameLoopStart = 0.0f;
    gameState->deltaTime = 0.0f;
    gameState->hideMeshes = 0;

    gameState->gravity = (Vec3d){0, -100, 0};

    setupPlayer(&gameState->player);
    setupCamera(&gameState->camera);
}
