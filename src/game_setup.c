#include "game_setup.h"

#include "game_state.h"

void setupGameState(GameState* gameState) {
    gameState->deltaTime = 0.05;
    gameState->hideMeshes = 0;

    gameState->gravity.x = 0;
    gameState->gravity.y = -100;
    gameState->gravity.z = 0;

    gameState->player.movementSpeed = 200;

    gameState->player.position.x = 0;
    gameState->player.position.y = 0;
    gameState->player.position.z = 0;

    gameState->player.velocity.x = 0;
    gameState->player.velocity.y = 0;
    gameState->player.velocity.z = 0;
}
