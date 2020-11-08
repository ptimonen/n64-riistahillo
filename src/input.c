#include "input.h"

#include <nusys.h>
#include "game_state.h"

void updateInput(struct GameState* gameState) {
    static NUContData controllerData;
    Player* player = &gameState->player;

    nuContDataGetEx(&controllerData, 0);

    // Values 61 and 63 based on nusys.h
    player->movementControl = (Vec2f) {
        controllerData.stick_x / 61,
        controllerData.stick_y / 63
    };

    if (controllerData.button & B_BUTTON) {
        gameState->hideMeshes = TRUE;
    } else {
        gameState->hideMeshes = FALSE;
    }
}