#include "render.h"

#include <Assert.h>
#include <nusys.h>
#include <ultra64.h>
#include <gbi.h>
#include <math.h>

#include "models/enemy/enemy.h"
#include "models/enemy/boom0.h"
#include "models/enemy/boom1.h"
#include "models/enemy/boom2.h"
#include "models/enemy/boom3.h"
#include "models/enemy/boom4.h"
#include "models/placeholder_sphere/placeholder_sphere.h"
#include "models/players/mask_player_1.h"
#include "models/players/mask_player_2.h"
#include "models/players/mask_player_3.h"
#include "models/players/mask_player_4.h"
#include "models/players/drum.h"
#include "models/players/drum_waves.h"
#include "models/players/drum_waves_low.h"
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
#include "models/end/skull.h"
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

void pushLookAt(GraphicsTask* graphicsTask, Vec3f eye, Vec3f target, Vec3f up) {
    guLookAt(&graphicsTask->objectTransforms[graphicsTask->matrixIndex],
             eye.x, eye.y, eye.z, target.x, target.y, target.z, up.x, up.y, up.z);
    gSPMatrix(
        g_dl++,
        OS_K0_TO_PHYSICAL(&graphicsTask->objectTransforms[graphicsTask->matrixIndex]),
        G_MTX_MODELVIEW | G_MTX_PUSH | G_MTX_MUL
    );
    ++graphicsTask->matrixIndex;
}

void pushRotation(GraphicsTask* graphicsTask, float angle, Vec3f axis) {
    guRotate(&graphicsTask->objectTransforms[graphicsTask->matrixIndex], angle, axis.x, axis.y, axis.z);
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

void drawCharacter(GraphicsTask* graphicsTask, Vec2f position, float scale, const Player* player) {
    pushTransform(
        graphicsTask,
        -player->movementControl.y * 15.0f,
        player->movementControl.x * 15.0f,
        0.0f,
        scale,
        position.x,
        position.y,
        0.0f
    );

    if(player->index == 0) {
        drawTexturedModel(Wtx_mask_player_1);
    }
    else if(player->index == 1) {
        drawTexturedModel(Wtx_mask_player_2);
    }
    else if(player->index == 2) {
        drawTexturedModel(Wtx_mask_player_3);
    }
    else if(player->index == 3) {
        drawTexturedModel(Wtx_mask_player_4);
    }

    popTransform();
}

void drawDrum(GraphicsTask* graphicsTask, Vec2f chainPosition, Vec2f position, float scale, float speed) {
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
    if(speed > DAMAGE_THRESHOLD_SPEED) {
        drawTexturedModel(Wtx_drum_waves);
    }
    else if(speed > DAMAGE_THRESHOLD_SPEED_LOW) {
        drawTexturedModel(Wtx_drum_waves_low);
    }
    popTransform();
}

void drawChainWith(GraphicsTask* graphicsTask, const Chain* chain, int r, int g, int b, float thickness, int z) {
    Vtx_t* vertexBuffer = graphicsTask->vertexBuffers[graphicsTask->vertexBufferIndex];
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

        vertexBuffer[i * 4 + 0] = (Vtx_t) {v0.x, v0.y, z, 0, 0, 0, r, g, b, a};
        vertexBuffer[i * 4 + 1] = (Vtx_t) {v1.x, v1.y, z, 0, 0, 0, r, g, b, a};
        vertexBuffer[i * 4 + 2] = (Vtx_t) {v2.x, v2.y, z, 0, 0, 0, r, g, b, a};
        vertexBuffer[i * 4 + 3] = (Vtx_t) {v3.x, v3.y, z, 0, 0, 0, r, g, b, a};
    }

    gSPVertex(g_dl++, vertexBuffer, 4 * n, 0);

    // Draw segments
    for (i = 0; i < n - 1; ++i) {
        int x = i * 4;
        gSP2Triangles(g_dl++, x + 0, x + 1, x + 2, 0, x + 2, x + 1, x + 3, 0);
    }

    // Fill gaps between segments
    for (i = 0; i < n - 2; ++i) {
        int x = i * 4;
        gSP2Triangles(g_dl++, x + 4, x + 2, x + 3, 0, x + 3, x + 5, x + 2, 0);
    }

    ++graphicsTask->vertexBufferIndex;
}

void drawChain(GraphicsTask* graphicsTask, const Chain* chain) {
    // Draw twice for cel shading.
    drawChainWith(graphicsTask, chain, 0, 0, 0, 35.0f, -3);
    drawChainWith(graphicsTask, chain, 200, 127, 0, 10.0f, 0);
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

void drawScoreUI(GraphicsTask* graphicsTask, Player* player)
{
    float x = -850.0f + (655.0f * player->index);
    int i = 0;
    int score = player->score;
    while (score > 0) {
        int digit = score % 10;
        pushTransform(
                graphicsTask,
                0.0f,
                -90.0f,
                0.0f,
                1.6f,
                x - (75.0f * i),
                650.0f,
                100.0f
        );
        drawOneNumber(digit);
        popTransform();

        score /= 10;
        ++i;
    }
}

void drawLevel(GraphicsTask* graphicsTask, float deltaTime)
{
    int i;
    menuFloating += 0.3f * deltaTime;

    for(i = 0; i < 4; ++i) {
        pushTransform(
                graphicsTask,
                0.0f, // roll
                -90.0f, // pitch
                cosf(menuFloating + (0.25 * i)) * 15.0f, // heading
                1.0f, // scale
                0.0f,
                cosf(menuFloating) * 10.0f,
                0.0f
        );

        if(i == 0) {
            drawTexturedModel(Wtx_level);
        }
        else if(i == 1) {
            drawTexturedModel(Wtx_level1);
        }
        else if(i == 2) {
            drawTexturedModel(Wtx_level2);
        }
        else {
            drawTexturedModel(Wtx_level3);
        }
        popTransform();
    }
}

void drawPlayer(GraphicsTask* graphicsTask, const Player* player) {
    const Chain* chain = &player->chain;
    if((int)(player->invulnerabilityTimer * PLAYER_INVULNERABILITY_BLINK_HZ) % 2 == 0) {
        drawCharacter(graphicsTask, chain->nodes[0].position, 2.3f, player);
    }
    drawDrum(
        graphicsTask,
        chain->nodes[chain->nodeCount - 2].position,
        chain->nodes[chain->nodeCount - 1].position,
        chain->nodes[chain->nodeCount - 1].radius / 25.0f,
        player->barrelSpeed
    );
    drawChain(graphicsTask, &player->chain);
    drawHearts(graphicsTask, player);
    drawScoreUI(graphicsTask, player);
}

void drawEnemy(GraphicsTask* graphicsTask, const Enemy* enemy, GameState* gameState) {
    float scale = 0.023f * enemy->verletBody.radius;
    Vec2f moveDirection = normalize2f(
        sub2f(
            player_getCharacter(&gameState->players[enemy->targetPlayerIndex])->position,
            enemy->verletBody.position
        )
    );
    Vec2f vectorWithBob = sub2f(enemy->verletBody.position, enemy->verletBody.oldPosition);
    Vec2f weightedMoveDirection = normalize2f(add2f(mul2f(moveDirection, 0.95f), (mul2f(vectorWithBob, 0.05f))));
    pushTransform(
        graphicsTask,
        0.0f, // roll
        enemy->boomTimer > 0.0f ? -90.0f : 0.0f, // pitch
        enemy->rotation, // heading
        scale,
        enemy->verletBody.position.x,
        enemy->verletBody.position.y,
        0.0f
    );
    // Makes enemy look at camera to compensate for camera perspective.
    pushRotation(
        graphicsTask,
        enemy->boomTimer > 0.0f ? 0.0f : -radToDeg(atan(length2f(enemy->verletBody.position) / gameState->camera.position.z)),
        (Vec3f) {-enemy->verletBody.position.y, enemy->verletBody.position.x, 0.0f}
    );
    // Makes enemy look at move direction.
    pushRotation(
        graphicsTask,
        enemy->boomTimer > 0.0f ? 0.0f : 15.0f,
        (Vec3f) {-weightedMoveDirection.y, weightedMoveDirection.x, 0.0f}
    );
    if(enemy->boomTimer <= 0.0f) {
        drawTexturedModel(Wtx_enemy);
    }
    else if(enemy->boomTimer <= 0.1f){
        drawTexturedModel(Wtx_boom4);
    }
    else if(enemy->boomTimer <= 0.2f){
        drawTexturedModel(Wtx_boom3);
    }
    else if(enemy->boomTimer <= 0.3f){
        drawTexturedModel(Wtx_boom2);
    }
    else if(enemy->boomTimer <= 0.4f){
        drawTexturedModel(Wtx_boom1);
    }
    else{
        drawTexturedModel(Wtx_boom0);
    }
    popTransform();
    popTransform();
    popTransform();
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

//    nuDebConTextColor(0, NU_DEB_CON_TEXT_GREEN);
    // nuDebConDisp(NU_SC_NOSWAPBUFFER);
    nuGfxTaskStart(
        graphicsTask->displayList,
        (int) (g_dl - graphicsTask->displayList) * sizeof(Gfx),
        NU_GFX_UCODE_F3DEX, // load the 'F3DEX' version graphics microcode, which runs on the RCP to process this display list
        NU_SC_SWAPBUFFER // tells NuSystem to immediately display the frame on screen after the RCP finishes rendering it
    );
//    nuDebConClear(0);
}

void renderGame(GraphicsTask* graphicsTask, struct GameState* gameState) {
    int i;
    if (!gameState->hideMeshes) {
        drawLevel(graphicsTask, gameState->deltaTime);
        for(i = 0; i < MAX_PLAYERS; ++i) {
            if (player_exists(&gameState->players[i])) {
                drawPlayer(graphicsTask, &gameState->players[i]);
            }
        }
        for(i = 0; i < MAX_ENEMIES; ++i) {
            if (enemy_exists(&gameState->enemies[i])) {
                drawEnemy(graphicsTask, &gameState->enemies[i], gameState);
            }
        }
    }
}

void renderMenu(GraphicsTask* graphicsTask, struct GameConfig* gameConfig, struct GameState* gameState) {

    menuFloating += 5.0f * gameState->deltaTime;
    menuRotation += 33.0f * gameState->deltaTime;
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
        3.75f, // scale
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
            40.0f, // pitch
            -20.0f, // heading
            6.0f, // scale
            -900.0f, // x
            650.0f - cosf(menuFloating) * 25.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_mask_player_1);
    popTransform();

    pushTransform(
            graphicsTask,
            25.0f, // roll
            40.0f, // pitch
            20.0f, // heading
            6.0f, // scale
            -950.0f, // x
            50.0f - cosf(menuFloating + 0.5f) * 25.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_mask_player_2);
    popTransform();

    pushTransform(
            graphicsTask,
            -25.0f, // roll
            -30.0f, // pitch
            20.0f, // heading
            6.0f, // scale
            950.0f, // x
            -50.0f + cosf(menuFloating + 0.25f) * 25.0f, // y
            0.0f  // z
    );
    drawTexturedModel(Wtx_mask_player_3);
    popTransform();

    pushTransform(
            graphicsTask,
            -20.0f, // roll
            -30.0f, // pitch
            -20.0f, // heading
            6.0f, // scale
            900.0f, // x
            -650.0f + cosf(menuFloating + 0.75f) * 25.0f, // y
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
                1.875f,
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
                    1.875f,
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

    if(player->health <= 0) {
        pushTransform(
                graphicsTask,
                0.0f,
                -90.0f,
                0.0f,
                1.875f,
                400.0f,
                170.0f - (228.0f * player->index),
                0.0f
        );
        drawTexturedModel(Wtx_skull);
        popTransform();
    }
}

void renderEnd(GraphicsTask* graphicsTask, struct GameState* gameState) {
    int i;

    menuFloating += 5.0f * gameState->deltaTime;
    menuRotation += 33.0f * gameState->deltaTime;
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
            1.875f,
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

    for(i = 0; i < 3; ++i) {
        pushTransform(
                graphicsTask,
                70.0f + (30.0f * i),
                -60.0f,
                20.0f + (15.0f * i) + menuRotation,
                4.4f,
                -850.0f,
                -650.0f + (650.0f * i) + cosf(menuFloating + (0.33f * i)) * 30.0f,
                0.0f
        );
        drawTexturedModel(Wtx_drum);
        popTransform();

        pushTransform(
                graphicsTask,
                70.0f + (30.0f * i),
                -120.0f,
                20.0f + (15.0f * i) + menuRotation,
                4.4f,
                850.0f,
                -650.0f + (650.0f * i) - cosf(menuFloating + (0.33f * i)) * 30.0f,
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

    if(programState->gameState.camera.screenShake > 3000.0f) {
        programState->gameState.camera.screenShake = 3000.0f;
    }
    if(programState->gameState.camera.screenShake > 0) {
        programState->gameState.camera.screenShake -= 2000.0f * programState->gameState.deltaTime;
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
