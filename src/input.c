#include "input.h"

#include <nusys.h>
#include "game_state.h"

void updateInput(struct GameState* gameState) {
    static NUContData controllerData;
    Character* character = &gameState->player.character;

    nuContDataGetEx(&controllerData, 0);

    {
        // Values 61 and 63 based on nusys.h
        float stickX = controllerData.stick_x / 61;
        float stickY = controllerData.stick_y / 63;

        character->velocity.x += stickX * gameState->deltaTime * character->movementSpeed;
        character->velocity.y += stickY * gameState->deltaTime * character->movementSpeed;
    }

    if (controllerData.trigger & A_BUTTON) {
        character->velocity = (Vec3d){0, 0, 0};
    }

    if (controllerData.button & B_BUTTON) {
        gameState->hideMeshes = TRUE;
    } else {
        gameState->hideMeshes = FALSE;
    }
}