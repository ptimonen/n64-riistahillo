#include "input.h"

#include <nusys.h>
#include "game_state.h"

NUContData g_controllerData;

void updateInput(struct GameState* gameState) {
    nuContDataGetEx(&g_controllerData, 0);

    {
        // Values 61 and 63 based on nusys.h
        float stickX = g_controllerData.stick_x / 61;
        float stickY = g_controllerData.stick_y / 63;

        gameState->player.velocity.x += stickX * gameState->deltaTime * gameState->player.movementSpeed;
        gameState->player.velocity.y += stickY * gameState->deltaTime * gameState->player.movementSpeed;
    }

    if (g_controllerData.trigger & A_BUTTON) {
        gameState->player.velocity.x = 0;
        gameState->player.velocity.y = 0;
        gameState->player.velocity.z = 0;
    }

    if (g_controllerData.button & B_BUTTON) {
        gameState->hideMeshes = TRUE;
    } else {
        gameState->hideMeshes = FALSE;
    }
}