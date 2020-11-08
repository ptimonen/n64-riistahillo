#include "render.h"

#include <Assert.h>
#include <nusys.h>
#include <ultra64.h>
#include <gbi.h>

#include "models/placeholder_sphere/placeholder_sphere.h"
#include "game_state.h"
#include "graphics.h"

#define VERTEX_BUFFER_MAX_SIZE 32
#define UNPACK(vec) vec.x, vec.y, vec.z

void drawTexturedModel(Gfx* gfx) {
    gDPSetCycleType(g_dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(g_dl++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPTexture(g_dl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetCombineMode(g_dl++, G_CC_MODULATEI, G_CC_MODULATEI);
    gDPSetTextureFilter(g_dl++, G_TF_POINT);
    gSPClearGeometryMode(g_dl++, 0xFFFFFFFF);
    gSPSetGeometryMode(g_dl++, G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER);

    gSPDisplayList(g_dl++, gfx);
    gDPPipeSync(g_dl++);
}

void drawCharacter(GraphicsTask* graphicsTask, Vec2f position, float scale, int matrixIndex) {
    guPosition(
        &graphicsTask->objectTransforms[matrixIndex],
        0, // roll
        0.0f, // pitch
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

    drawTexturedModel(Wtx_placeholder_sphere);

    gSPPopMatrix(g_dl++, G_MTX_MODELVIEW);
}

void drawChain(GraphicsTask* graphicsTask, const Chain* chain) {
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

void drawPlayer(GraphicsTask* graphicsTask, const Player* player) {
    const Chain* chain = &player->chain;
    drawCharacter(graphicsTask, chain->nodes[0].position, 1.0f, 0);
    drawCharacter(graphicsTask, chain->nodes[chain->nodeCount - 1].position, 0.5f, 1);
    drawChain(graphicsTask, &player->chain);
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

void render(struct GameState* gameState) {
    GraphicsTask* graphicsTask = beginGraphicsTask();
    loadProjectionMatrix(graphicsTask);
    loadViewMatrix(graphicsTask, &gameState->camera);

    if (!gameState->hideMeshes) {
        drawPlayer(graphicsTask, &gameState->player);
    }

    endGraphicsTask(graphicsTask);
    // drawDebugInfo();
}