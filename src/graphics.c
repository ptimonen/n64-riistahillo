
#include <nusys.h>
#include "graphics.h"


int g_graphicsTaskNum = 0;
GraphicsTask g_graphicsTasks[MAX_GRAPHICS_TASKS];
Gfx *g_dl;

GraphicsTask *gfxSwitchTask() {
    GraphicsTask *nextTask;
    // switch the current graphics task
    g_graphicsTaskNum = (g_graphicsTaskNum + 1) % MAX_GRAPHICS_TASKS;
    nextTask = &g_graphicsTasks[g_graphicsTaskNum];
    g_dl = &nextTask->displayList[0];
    return nextTask;
}

/*
  The viewport structure
  The conversion from (-1,-1,-1)-(1,1,1).  The decimal part is 2-bit.
 */
static Vp vp = {
        SCREEN_WD * 2, SCREEN_HT * 2, G_MAXZ / 2, 0,    /* The scale factor  */
        SCREEN_WD * 2, SCREEN_HT * 2, G_MAXZ / 2, 0,    /* Move  */
};

/*
  The initialization of RDP
*/
Gfx setup_rdpstate[] = {
        gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT),
        gsDPSetColorDither(G_CD_BAYER),
        gsSPEndDisplayList(),
};

/*
  The initialization of RSP
*/
Gfx setup_rspstate[] = {
        gsSPViewport(&vp),
        gsSPClearGeometryMode(0xFFFFFFFF),
        gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK),
        gsSPTexture(0, 0, 0, 0, G_OFF),
        gsSPEndDisplayList(),
};

// The initialization of RSP/RDP
void gfxRCPInit(void) {
    // set the RSP segment register
    gSPSegment(g_dl++, 0, 0x0);  // For the CPU virtual address

    // init the RSP
    gSPDisplayList(g_dl++, OS_K0_TO_PHYSICAL(setup_rspstate));

    // init the RDP
    gSPDisplayList(g_dl++, OS_K0_TO_PHYSICAL(setup_rdpstate));
}


// clears the frame buffer and z buffer
void gfxClearCfb(void) {
    // Clear the Z-buffer
    gDPSetDepthImage(g_dl++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetCycleType(g_dl++, G_CYC_FILL);
    gDPSetColorImage(g_dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
                     OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetFillColor(g_dl++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16 |
                                       GPACK_ZDZ(G_MAXFBZ, 0)));
    gDPFillRectangle(g_dl++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPPipeSync(g_dl++);

    // Clear the frame buffer
    gDPSetColorImage(g_dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
                     osVirtualToPhysical(nuGfxCfb_ptr));
    gDPSetFillColor(g_dl++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 |
                                       GPACK_RGBA5551(0, 0, 0, 1)));
    gDPFillRectangle(g_dl++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPPipeSync(g_dl++);
}
