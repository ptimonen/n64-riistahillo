#include "input.h"

#include <nusys.h>
#include "game_state.h"
#include "audio.h"
#include "globals.h"

void updateGameInput(struct GameState* gameState) {
    for(i = 0; i < 4; ++i) {
        if (gameState->players[i].health > 0) {
            updatePlayerInput(&gameState->players[i]);
        }
    }
}

void updatePlayerInput(struct Player* player) {
    static NUContData controllerData;

    nuContDataGetEx(&controllerData, player->index);

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

void updateMenuInput(struct ProgramState* programState) {
    GameConfig* gameConfig = &programState->gameConfig;
    static NUContData controllerData;
    nuContDataGetEx(&controllerData, 0);

    if (controllerData.trigger & L_JPAD && gameConfig->playerCount > 1) {
        gameConfig->playerCount--;
    }
    if (controllerData.trigger & R_JPAD && gameConfig->playerCount < 4) {
        gameConfig->playerCount++;
    }
    if (controllerData.trigger & U_JPAD) {
        gameConfig->gameMode = SURVIVAL;
    }
    if (controllerData.trigger & D_JPAD) {
        gameConfig->gameMode = BATTLE;
    }
    if(gameConfig->playerCount == 1 && gameConfig->gameMode == BATTLE) {
        gameConfig->playerCount = 2;
    }
    if (controllerData.trigger & A_BUTTON) {
        programState->activeScreen = LOADING;
    }
}