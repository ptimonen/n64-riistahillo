#include <nusys.h>

#include "globals.h"
#include "game_loop.h"
#include "game_setup.h"

void mainproc(void)
{
  // initialize the graphics system
  nuGfxInit();

  // initialize the controller manager
  nuContInit();

  // initialize the level
  setupGameState(&g_gameState);

  // set the update+draw callback to be called every frame
  nuGfxFuncSet((NUGfxFunc)tick);

  // enable video output
  nuGfxDisplayOn();

  // send this thread into an infinite loop
  while(1)
    ;
}


