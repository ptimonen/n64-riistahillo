#include "input.h"

#include <nusys.h>
#include "game_state.h"
#include "audio.h"

void updateInput(struct GameState* gameState) {
    static NUContData controllerData;
    Player* player = &gameState->player;

    nuContDataGetEx(&controllerData, 0);

    // Values 61 and 63 based on nusys.h
    player->movementControl = (Vec2f) {
        controllerData.stick_x / 61.0f,
        controllerData.stick_y / 63.0f
    };

    player->movementControl.x = clamp(player->movementControl.x, -1, +1);
    player->movementControl.y = clamp(player->movementControl.y, -1, +1);

    if (controllerData.button & B_BUTTON) {
        VerletBody* character = player_getCharacter(player);
        character->isStatic = TRUE;
        character->oldPosition = character->position;
    } else {
        player_getCharacter(player)->isStatic = FALSE;
    }

    if (controllerData.button & A_BUTTON) {
        VerletBody* boulder = player_getBoulder(player);
        boulder->isStatic = TRUE;
        boulder->oldPosition = boulder->position;
    } else {
        player_getBoulder(player)->isStatic = FALSE;
    }

    if(controllerData.trigger & A_BUTTON) {
        sndHandle = nuAuStlSndPlayerPlay(SND_DRUM);
    }
}