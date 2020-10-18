#include "render.h"

#include <Assert.h>
#include <nusys.h>

#include "models/placeholder_sphere/placeholder_sphere.h"
#include "game_state.h"
#include "graphics.h"

Vec3d cameraPos = {0.0f, 0.0f, 2400.0f};
Vec3d cameraTarget = {0.0f, 0.0f, 0.0f};
Vec3d cameraUp = {0.0f, 1.0f, 0.0f};

// this is an example of rendering a model defined as a set of static display lists
void drawModel() {
    gDPSetCycleType(g_dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(g_dl++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPTexture(g_dl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetCombineMode(g_dl++, G_CC_MODULATEI, G_CC_MODULATEI);
    gDPSetTextureFilter(g_dl++, G_TF_POINT);
    gSPClearGeometryMode(g_dl++, 0xFFFFFFFF);
    gSPSetGeometryMode(g_dl++, G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER);

    gSPDisplayList(g_dl++, Wtx_placeholder_sphere);
    gDPPipeSync(g_dl++);
}

void render(struct GameState *gameState) {
    unsigned short perspNorm;
    GraphicsTask *gfxTask;

    // switch the current graphics task
    // also updates the g_dl global variable
    gfxTask = gfxSwitchTask();

    gfxRCPInit();
    gfxClearCfb();

    guPerspective(&gfxTask->projection, &perspNorm, FOVY, ASPECT, NEAR_PLANE,
                  FAR_PLANE, 1.0);

    gSPPerspNormalize(g_dl++, perspNorm);

    guLookAt(&gfxTask->modelview, cameraPos.x, cameraPos.y,
             cameraPos.z, cameraTarget.x, cameraTarget.y,
             cameraTarget.z, cameraUp.x, cameraUp.y, cameraUp.z);

    gSPMatrix(
            g_dl++,
            // we use the OS_K0_TO_PHYSICAL macro to convert the pointer to this matrix
            // into a 'physical' address as required by the RCP
            OS_K0_TO_PHYSICAL(&(gfxTask->projection)),
            // these flags tell the graphics microcode what to do with this matrix
            // documented here: http://n64devkit.square7.ch/tutorial/graphics/1/1_3.htm
            G_MTX_PROJECTION | // using the projection matrix stack...
            G_MTX_LOAD | // don't multiply matrix by previously-top matrix in stack
            G_MTX_NOPUSH // don't push another matrix onto the stack before operation
    );

    gSPMatrix(g_dl++,
              OS_K0_TO_PHYSICAL(&(gfxTask->modelview)),
            // similarly this combination means "replace the modelview matrix with this new matrix"
              G_MTX_MODELVIEW | G_MTX_NOPUSH | G_MTX_LOAD
    );

    guPosition(
            &gfxTask->objectTransforms[0],
            // rotation
            0, // roll
            0.0f, // pitch
            0.0f, // heading
            1.0f, // scale
            // position
            gameState->player.position.x,
            gameState->player.position.y,
            gameState->player.position.z
    );

    // push relative transformation matrix
    gSPMatrix(g_dl++,
              OS_K0_TO_PHYSICAL(&(gfxTask->objectTransforms[0])),
              G_MTX_MODELVIEW | // operating on the modelview matrix stack...
              G_MTX_PUSH | // ...push another matrix onto the stack...
              G_MTX_MUL // ...which is multipled by previously-top matrix (eg. a relative transformation)
    );

    if (!gameState->hideMeshes) {
        drawModel();
    }

    // pop the matrix that we added back off the stack, to move the drawing position
    // back to where it was before we rendered this object
    gSPPopMatrix(g_dl++, G_MTX_MODELVIEW);

// mark the end of the display list
    gDPFullSync(g_dl++);
    gSPEndDisplayList(g_dl++);

    assert(g_dl - gfxTask->displayList < MAX_DISPLAY_LIST_COMMANDS);

// create a graphics task to render this displaylist and send it to the RCP
    nuGfxTaskStart(
            gfxTask->displayList,
            (int) (g_dl - gfxTask->displayList) * sizeof(Gfx),
            NU_GFX_UCODE_F3DEX, // load the 'F3DEX' version graphics microcode, which runs on the RCP to process this display list
            NU_SC_SWAPBUFFER // tells NuSystem to immediately display the frame on screen after the RCP finishes rendering it
    );
}