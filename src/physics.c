#include "physics.h"

#include "game_state.h"

void updatePhysics(struct GameState* gameState) {
    addTo(&gameState->player.velocity, mul(gameState->gravity, gameState->deltaTime));
    addTo(&gameState->player.position, mul(gameState->player.velocity, gameState->deltaTime));
}