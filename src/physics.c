#include "physics.h"

#include "game_state.h"

void updatePhysics(struct GameState* gameState) {
    Character* character = &gameState->player.character;

    addTo(&character->velocity, mul(gameState->gravity, gameState->deltaTime));
    addTo(&character->position, mul(character->velocity, gameState->deltaTime));
}