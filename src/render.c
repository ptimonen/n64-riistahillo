#include "render.h"

#include <Assert.h>
#include <nusys.h>
#include <ultra64.h>
#include <gbi.h>
#include <math.h>

#include "models/placeholder_sphere/placeholder_sphere.h"
#include "models/players/mask_player_1.h"
#include "models/players/mask_player_2.h"
#include "models/players/mask_player_3.h"
#include "models/players/mask_player_4.h"
#include "models/players/drum.h"
#include "models/level/level.h"
#include "models/level/level1.h"
#include "models/level/level2.h"
#include "models/level/level3.h"
#include "models/menu/background.h"
#include "models/menu/mode_battle.h"
#include "models/menu/mode_survival.h"
#include "models/menu/players_1.h"
#include "models/menu/players_2.h"
#include "models/menu/players_3.h"
#include "models/menu/players_4.h"
#include "models/menu/text_modes.h"
#include "models/menu/text_players.h"
#include "models/gui/heart_p1.h"
#include "models/gui/heart_p2.h"
#include "models/gui/heart_p3.h"
#include "models/gui/heart_p4.h"
#include "models/end/end_background.h"
#include "models/end/number_0.h"
#include "models/end/number_1.h"
#include "models/end/number_2.h"
#include "models/end/number_3.h"
#include "models/end/number_4.h"
#include "models/end/number_5.h"
#include "models/end/number_6.h"
#include "models/end/number_7.h"
#include "models/end/number_8.h"
#include "models/end/number_9.h"
#include "models/end/text_score.h"
#include "models/end/text_scores.h"
#include "game_state.h"
#include "graphics.h"
#include "globals.h"

#define UNPACK(vec) vec.x, vec.y, vec.z

void drawTexturedModel(Gfx* gfx) {
    gSPDisplayList(g_dl++, gfx);
    gDPPipeSync(g_dl++);
}

void pushTransform(GraphicsTask* graphicsTask, float roll, float pitch, float heading, float scale, float x, float y, float z) {
    guPosition(&graphicsTask->objectTransforms[graphicsTask->matrixIndex], roll, pitch, heading, scale, x, y, z);
    gSPMatrix(
        g_dl++,
        OS_K0_TO_PHYSICAL(&graphicsTask->objectTransforms[graphicsTask->matrixIndex]),
        G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL
    );
    ++graphicsTask->matrixIndex;
}

void popTransform() {
    gSPPopMatrix(g_dl++, G_MTX_MODELVIEW);
}

void drawCharacter(GraphicsTask* graphicsTask, Vec2f position, float scale, int playerIndex) {
    pushTransform(
        graphicsTask,
        0, // roll
        -90.0f, // pitch
        0.0f, // heading
        scale,
        position.x,
        position.y,
        0
    );

    if(playerIndex == 0) {
        drawTexturedModel(Wtx_mask_player_1);
    }
    else if(playerIndex == 1) {
        drawTexturedModel(Wtx_mask_player_2);
    }
    else if(playerIndex == 2) {
        drawTexturedModel(Wtx_mask_player_3);
    }
    else if(playerIndex == 3) {
        drawTexturedModel(Wtx_mask_player_4);
    }

    popTransform();
}

void drawDrum(GraphicsTask* graphicsTask, Vec2f chainPosition, Vec2f position, float scale) {
    Vec2f direction = sub2f(chainPosition, position);
    float angle = radToDeg(atan2(direction.y, direction.x));
    pushTransform(
        graphicsTask,
        angle, // roll
        -90.0f, // pitch
        0.0f, // heading
        scale,
        position.x,
        position.y,
        0
    );

    drawTexturedModel(Wtx_drum);

    popTransform();
}

// TODO: Use player index to draw all chains
void drawChain(GraphicsTask* graphicsTask, const Chain* chain) {
    Vtx_t* vertexBuffer = graphicsTask->vertexBuffers[graphicsTask->vertexBufferIndex];
    const float thickness = 10;
    const int r = 200;
    const int g = 127;
    const int b = 0;
    const int a = 255;
    int n = MIN(VERTEX_BUFFER_MAX_SIZE / 4, chain->nodeCount);
    int i;

    for (i = 0; i < n - 1; ++i) {
        Vec2f start = chain->nodes[i].position;
        Vec2f end = chain->nodes[i + 1].position;
        Vec2f delta = normalize2f(sub2f(end, start));
        Vec2f normal = mul2f((Vec2f) {delta.y, -delta.x}, -0.5f * thickness);

        Vec2f v0 = add2f(start, normal);
        Vec2f v1 = sub2f(start, normal);
        Vec2f v2 = add2f(end, normal);
        Vec2f v3 = sub2f(end, normal);

        vertexBuffer[i * 4 + 0] = (Vtx_t) {v0.x, v0.y, 0, 0, 0, 0, r, g, b, a};
        vertexBuffer[i * 4 + 1] = (Vtx_t) {v1.x, v1.y, 0, 0, 0, 0, r, g, b, a};
        vertexBuffer[i * 4 + 2] = (Vtx_t) {v2.x, v2.y, 0, 0, 0, 0, r, g, b, a};
        vertexBuffer[i * 4 + 3] = (Vtx_t) {v3.x, v3.y, 0, 0, 0, 0, r, g, b, a};
    }

    gSPVertex(g_dl++, vertexBuffer, 4 * n, 0);

    for (i = 0; i < n - 1; ++i) {
        int x = i * 4;
        gSP2Triangles(g_dl++, x + 0, x + 1, x + 2, 0, x + 2, x + 1, x + 3, 0);
    }

    ++graphicsTask->vertexBufferIndex;
}

void drawHearts(GraphicsTask* graphicsTask, const Player* player) {
    float x = -1100.0f + (650.0f * player->index);
    float y = 800.0f;
    int i;

    for(i = 0; i < player->health; ++i) {
        pushTransform(
                graphicsTask,
                0.0f, // roll
                -90.0f, // pitch
                0.0f, // heading
                4.0f, // scale
                x + (125.0f * i),
                y,
                100.0f
        );

        if(player->index == 0) {
             drawTexturedModel(Wtx_heart_p1);
        }
        else if(player->index == 1) {
            drawTexturedModel(Wtx_heart_p2);
        }
        else if(player->index == 2) {
            drawTexturedModel(Wtx_heart_p3);
        }
        else {
            drawTexturedModel(Wtx_heart_p4);
        }
        popTransform();
    }
}

void drawLevel(GraphicsTask* graphicsTask)
{
    pushTransform(
            graphicsTask,
            0.0f, // roll
            -90.0f, // pitch
            0.0f, // heading
            1.0f, // scale
            0.0f,
            0.0f,
            0.0f
    );

    drawTexturedModel(Wtx_level);
    drawTexturedModel(Wtx_level1);
    drawTexturedModel(Wtx_level2);
    drawTexturedModel(Wtx_level3);
    popTransform();
}

void drawPlayer(GraphicsTask* graphicsTask, const Player* player) {
    const Chain* chain = &player->chain;
    if((int)(player->invulnerabilityTimer * PLAYER_INVULNERABILITY_BLINK_HZ) % 2 == 0) {
        drawCharacter(graphicsTask, chain->nodes[0].position, 2.3f, player->index);
    }
    drawDrum(graphicsTask, chain->nodes[chain->nodeCount - 2].position, chain->nodes[chain->nodeCount - 1].position, 2.0f);
    drawChain(graphicsTask, &player->chain);
    drawHearts(graphicsTask, player);
}

void drawDebugInfo() {
    nuDebConTextColor(0, NU_DEB_CON_TEXT_GREEN);
    nuDebConDisp(NU_SC_SWAPBUFFER);
    nuDebConClear(0);
}

void loadProjectionMatrix(GraphicsTask* graphicsTask) {
    unsigned short perspNorm;
    guPerspective(
        &graphicsTask->projection,
        &perspNorm,
        FOVY,
        ASPECT,
        NEAR_PLANE,
        FAR_PLANE,
        1.0
    );
    gSPPerspNormalize(g_dl++, perspNorm);

    gSPMatrix(
        g_dl++,
        OS_K0_TO_PHYSICAL(&graphicsTask->projection),
        G_MTX_PROJECTION | G_MTX_NOPUSH | G_MTX_LOAD
    );
}

void loadViewMatrix(GraphicsTask* graphicsTask, const Camera* camera) {
    guLookAt(
        &graphicsTask->modelview,
        UNPACK(camera->position),
        UNPACK(camera->target),
        UNPACK(camera->up)
    );

    gSPMatrix(
        g_dl++,
        OS_K0_TO_PHYSICAL(&graphicsTask->modelview),
        G_MTX_MODELVIEW | G_MTX_NOPUSH | G_MTX_LOAD
    );
}

GraphicsTask* beginGraphicsTask() {
    GraphicsTask* graphicsTask = gfxSwitchTask();
    gfxRCPInit();
    gfxClearCfb();
    return graphicsTask;
}

void endGraphicsTask(GraphicsTask* graphicsTask) {
    gDPFullSync(g_dl++);
    gSPEndDisplayList(g_dl++);

    assert(g_dl - graphicsTask->displayList < MAX_DISPLAY_LIST_COMMANDS);

    nuGfxTaskStart(
        graphicsTask->displayList,
        (int) (g_dl - graphicsTask->displayList) * sizeof(Gfx),
        NU_GFX_UCODE_F3DEX, // load the 'F3DEX' version graphics microcode, which runs on the RCP to process this display list
        NU_SC_SWAPBUFFER // tells NuSystem to immediately display the frame on screen after the RCP finishes rendering it
    );
}

void renderGame(GraphicsTask* graphicsTask, struct GameState* gameState) {
    int i;
    if (!gameState->hideMeshes) {
        drawLevel(graphicsTask);
        for(i = 0; i < 4; ++i) {
            if (player_exists(&gameState->players[i])) {
                drawPlayer(graphicsTask, &gameState->players[i]);
            }
        }
    }
}

void renderMenu(GraphicsTask* graphicsTask, struct GameConfig* gameConfig, struct GameState* gameState) {

    menuRotation += 33.0f * gameState->physics.deltaTime;
    pushTransform(
            graphicsTask,
            0.0f, // roll
            menuRotation, // pitch
            0.0f, // heading
            15.0f, // scale
            0.0f, // x
            0.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_background);
    popTransform();

    pushTransform(
        graphicsTask,
        0.0f, // roll
        -90.0f, // pitch
        0.0f, // heading
        15.0f, // scale
        0.0f, // x
        0.0f, // y
        0.0f  // z
    );
    if (gameConfig->gameMode == BATTLE) {
        drawTexturedModel(Wtx_mode_battle);
    } else {
        drawTexturedModel(Wtx_mode_survival);
    }
    if (gameConfig->playerCount == 1) {
        drawTexturedModel(Wtx_players_1);
    } else if (gameConfig->playerCount == 2) {
        drawTexturedModel(Wtx_players_2);
    } else if (gameConfig->playerCount == 3) {
        drawTexturedModel(Wtx_players_3);
    } else {
        drawTexturedModel(Wtx_players_4);
    }
    drawTexturedModel(Wtx_text_modes);
    drawTexturedModel(Wtx_text_players);
    popTransform();

    pushTransform(
            graphicsTask,
            20.0f, // roll
            -50.0f, // pitch
            -20.0f, // heading
            6.0f, // scale
            -900.0f, // x
            650.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_mask_player_1);
    popTransform();

    pushTransform(
            graphicsTask,
            25.0f, // roll
            -50.0f, // pitch
            00.0f, // heading
            6.0f, // scale
            -950.0f, // x
            50.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_mask_player_2);
    popTransform();

    pushTransform(
            graphicsTask,
            -25.0f, // roll
            -120.0f, // pitch
            00.0f, // heading
            6.0f, // scale
            950.0f, // x
            -50.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_mask_player_3);
    popTransform();

    pushTransform(
            graphicsTask,
            -20.0f, // roll
            -120.0f, // pitch
            20.0f, // heading
            6.0f, // scale
            900.0f, // x
            -650.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_mask_player_4);
    popTransform();
}

void drawOneNumber(int number)
{
    if(number == 0) {
        drawTexturedModel(Wtx_number_0);
    }
    else if(number == 1) {
        drawTexturedModel(Wtx_number_1);
    }
    else if(number == 2) {
        drawTexturedModel(Wtx_number_2);
    }
    else if(number == 3) {
        drawTexturedModel(Wtx_number_3);
    }
    else if(number == 4) {
        drawTexturedModel(Wtx_number_4);
    }
    else if(number == 5) {
        drawTexturedModel(Wtx_number_5);
    }
    else if(number == 6) {
        drawTexturedModel(Wtx_number_6);
    }
    else if(number == 7) {
        drawTexturedModel(Wtx_number_7);
    }
    else if(number == 8) {
        drawTexturedModel(Wtx_number_8);
    }
    else if(number == 9) {
        drawTexturedModel(Wtx_number_9);
    }
}

void drawScore(GraphicsTask* graphicsTask, Player* player)
{
    int i = 0;
    int score = player->score;
    if(score == 0) {
        int digit = score % 10;
        pushTransform(
                graphicsTask,
                0.0f,
                -90.0f,
                0.0f,
                15.0f,
                200.0f,
                170.0f - (228.0f * player->index),
                0.0f
        );
        drawOneNumber(0);
        popTransform();
    }
    else {
        while (score > 0) {
            int digit = score % 10;
            pushTransform(
                    graphicsTask,
                    0.0f,
                    -90.0f,
                    0.0f,
                    15.0f,
                    200.0f - (100.0f * i),
                    170.0f - (228.0f * player->index),
                    0.0f
            );
            drawOneNumber(digit);
            popTransform();

            score /= 10;
            ++i;
        }
    }
}

void renderEnd(GraphicsTask* graphicsTask, struct GameState* gameState) {
    int i;

    menuRotation += 33.0f * gameState->physics.deltaTime;
    pushTransform(
            graphicsTask,
            -90.0f, // roll
            menuRotation, // pitch
            0.0f, // heading
            15.0f, // scale
            0.0f, // x
            0.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_end_background);
    popTransform();

    pushTransform(
            graphicsTask,
            0.0f,
            -90.0f,
            0.0f,
            15.0f,
            0.0f,
            0.0f,
            0.0f
    );
    drawTexturedModel(Wtx_text_score);
    drawTexturedModel(Wtx_text_scores);
    popTransform();

    for(i = 0; i < 4; ++i) {
        drawScore(graphicsTask, &gameState->players[i]);
    }

    for(i = 0; i < 4; ++i) {
        pushTransform(
                graphicsTask,
                70.0f + (30.0f * i),
                -60.0f,
                20.0f + (15.0f * i),
                4.4f,
                -850.0f,
                -650.0f + (650.0f * i),
                0.0f
        );
        drawTexturedModel(Wtx_drum);
        popTransform();

        pushTransform(
                graphicsTask,
                70.0f + (30.0f * i),
                -120.0f,
                20.0f + (15.0f * i),
                4.4f,
                850.0f,
                -650.0f + (650.0f * i),
                0.0f
        );
        drawTexturedModel(Wtx_drum);
        popTransform();
    }
}

void setRenderAttributes() {
    gDPSetCycleType(g_dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(g_dl++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gDPSetCombineMode(g_dl++, G_CC_MODULATEI, G_CC_MODULATEI);
    gDPSetTextureFilter(g_dl++, G_TF_BILERP);
    gSPTexture(g_dl++, 0x8000, 0x8000, 0, 0, G_ON);
    gSPClearGeometryMode(g_dl++, 0xFFFFFFFF);
    gSPSetGeometryMode(g_dl++, G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER | G_CULL_BACK);
    gDPPipeSync(g_dl++);
}

void render(struct ProgramState* programState) {
    float addScreenShake;
    GraphicsTask* graphicsTask = beginGraphicsTask();
    loadProjectionMatrix(graphicsTask);
    loadViewMatrix(graphicsTask, &programState->gameState.camera);
    setRenderAttributes();

    if(programState->gameState.camera.screenShake > 0) {
        programState->gameState.camera.screenShake -= 2000.0f * programState->gameState.physics.deltaTime;
    }
    else {
        programState->gameState.camera.screenShake = 0.0f;
    }

    // Get random between -1 and 1, multiply it with screen shake amount
    addScreenShake = -1.0f + ((float)RAND(200) / 100.0f);
    addScreenShake *= programState->gameState.camera.screenShake;

    programState->gameState.camera.position =
            (Vec3f){programState->gameState.camera.startPosition.x,
                    programState->gameState.camera.startPosition.y + addScreenShake,
                    programState->gameState.camera.startPosition.z};

    if (programState->activeScreen == GAME) {
        renderGame(graphicsTask, &programState->gameState);
    } else if(programState->activeScreen == MENU) {
        renderMenu(graphicsTask, &programState->gameConfig, &programState->gameState);
    }
    else {
        renderEnd(graphicsTask, &programState->gameState);
    }

    endGraphicsTask(graphicsTask);
    // drawDebugInfo();
}
