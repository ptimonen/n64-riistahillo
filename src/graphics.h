#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include <nusys.h>

// The screen size
#define SCREEN_HT        240
#define SCREEN_WD        320

#define MAX_GRAPHICS_TASKS 2
#define MAX_DISPLAY_LIST_COMMANDS 2048
#define MAX_OBJECTS 20

#define FOVY 45
#define ASPECT (f32)SCREEN_WD/(f32)SCREEN_HT
#define NEAR_PLANE 30
#define FAR_PLANE 3000

// a struct to hold graphics data used by the RCP which can change at runtime
typedef struct GraphicsTask {
  Mtx projection;
  Mtx modelview;
  Mtx objectTransforms[MAX_OBJECTS];
  Gfx displayList[MAX_DISPLAY_LIST_COMMANDS];
} GraphicsTask;

Gfx* g_dl;

GraphicsTask * gfxSwitchTask();
void gfxRCPInit();
void gfxClearCfb();

#endif /* _GRAPHIC_H_ */

