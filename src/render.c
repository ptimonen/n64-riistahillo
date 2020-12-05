#include "render.h"

#include <Assert.h>
#include <nusys.h>
#include <ultra64.h>
#include <gbi.h>
#include <math.h>

#include "models/placeholder_sphere/placeholder_sphere.h"
#include "models/players/mask_player_1.h"
#include "models/players/drum.h"
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
#include "game_state.h"
#include "graphics.h"
#include "globals.h"

#define VERTEX_BUFFER_MAX_SIZE 32
#define UNPACK(vec) vec.x, vec.y, vec.z

void drawTexturedModel(Gfx* gfx) {
    gDPSetCycleType(g_dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(g_dl++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPTexture(g_dl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetCombineMode(g_dl++, G_CC_MODULATEI, G_CC_MODULATEI);
    gDPSetTextureFilter(g_dl++, G_TF_BILERP);
    gSPClearGeometryMode(g_dl++, 0xFFFFFFFF);
    gSPSetGeometryMode(g_dl++, G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER);

    gSPDisplayList(g_dl++, gfx);
    gDPPipeSync(g_dl++);
}

void drawCharacter(GraphicsTask* graphicsTask, Vec2f position, float scale, int matrixIndex) {
    guPosition(
        &graphicsTask->objectTransforms[matrixIndex],
        0, // roll
        -90.0f, // pitch
        0.0f, // heading
        scale,
        position.x,
        position.y,
        0
    );

    gSPMatrix(
        g_dl++,
        OS_K0_TO_PHYSICAL(&graphicsTask->objectTransforms[matrixIndex]),
        G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL
    );

    drawTexturedModel(Wtx_mask_player_1);

    gSPPopMatrix(g_dl++, G_MTX_MODELVIEW);
}

void drawDrum(GraphicsTask* graphicsTask, Vec2f chainPosition, Vec2f position, float scale, int matrixIndex) {
    Vec2f direction = sub2f(chainPosition, position);
    float angle = radToDeg(atan2(direction.y, direction.x));
    guPosition(
        &graphicsTask->objectTransforms[matrixIndex],
        angle, // roll
        -90.0f, // pitch
        0.0f, // heading
        scale,
        position.x,
        position.y,
        0
    );

    gSPMatrix(
        g_dl++,
        OS_K0_TO_PHYSICAL(&graphicsTask->objectTransforms[matrixIndex]),
        G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL
    );

    drawTexturedModel(Wtx_drum);

    gSPPopMatrix(g_dl++, G_MTX_MODELVIEW);
}

// TODO: Use player index to draw all chains
void drawChain(GraphicsTask* graphicsTask, const Chain* chain, int playerIndex) {
    static Vtx_t vertexBuffer[VERTEX_BUFFER_MAX_SIZE];
    const float thickness = 10;
    const int r = 200;
    const int g = 127;
    const int b = 0;
    const int a = 255;
    int i;

    int n = MIN(VERTEX_BUFFER_MAX_SIZE / 4, chain->nodeCount);

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
}

void drawHearts(GraphicsTask* graphicsTask, const Player* player, int matrixIndex) {
    float x = -1100.0f + (525.0f * player->index);
    float y = 800.0f;

    for(i = 0; i < player->health; ++i) {
        guPosition(
                &graphicsTask->objectTransforms[matrixIndex + i],
                0.0f, // roll
                -90.0f, // pitch
                0.0f, // heading
                4.0f, // scale
                x + (125.0f * i),
                y,
                100.0f
        );

        gSPMatrix(
                g_dl++,
                OS_K0_TO_PHYSICAL(&graphicsTask->objectTransforms[matrixIndex + i]),
                G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL
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
        gSPPopMatrix(g_dl++, G_MTX_MODELVIEW);
    }
}

void drawPlayer(GraphicsTask* graphicsTask, const Player* player, int matrixIndex) {
    const Chain* chain = &player->chain;
    drawCharacter(graphicsTask, chain->nodes[0].position, 2.3f, matrixIndex);
    drawDrum(graphicsTask, chain->nodes[chain->nodeCount - 2].position, chain->nodes[chain->nodeCount - 1].position, 2.0f, matrixIndex + 4);
    drawChain(graphicsTask, &player->chain, player->index);
    //drawHearts(graphicsTask, player, matrixIndex + 8 + (player->index * 3));
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
    if (!gameState->hideMeshes) {
        for(i = 0; i < 4; ++i) {
            if (gameState->players[i].health > 0) {
                drawPlayer(graphicsTask, &gameState->players[i], i);
            }
        }
    }
}

void renderMenu(GraphicsTask* graphicsTask, struct GameConfig* gameConfig) {
    const int matrixIndex = 0;

    guPosition(
        &graphicsTask->objectTransforms[matrixIndex],
        0.0f, // roll
        -90.0f, // pitch
        0.0f, // heading
        15.0f, // scale
        0.0f, // x
        0.0f, // y
        0.0f  // z
    );

    gSPMatrix(
        g_dl++,
        OS_K0_TO_PHYSICAL(&graphicsTask->objectTransforms[matrixIndex]),
        G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL
    );

    drawTexturedModel(Wtx_background);
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

    gSPPopMatrix(g_dl++, G_MTX_MODELVIEW);
}

void render(struct ProgramState* programState) {
    GraphicsTask* graphicsTask = beginGraphicsTask();
    loadProjectionMatrix(graphicsTask);
    loadViewMatrix(graphicsTask, &programState->gameState.camera);

    if (programState->activeScreen == GAME) {
        renderGame(graphicsTask, &programState->gameState);
    } else {
        renderMenu(graphicsTask, &programState->gameConfig);
    }

    endGraphicsTask(graphicsTask);
    // drawDebugInfo();
}
