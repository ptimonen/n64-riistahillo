#ifndef RENDER_H
#define RENDER_H

struct ProgramState;
struct GameState;
struct GameConfig;
struct GraphicsTask;

void render(struct ProgramState* programState);

void renderGame(struct GraphicsTask* graphicsTask, struct GameState* gameState);

void renderMenu(struct GraphicsTask* graphicsTask, struct GameConfig* gameConfig);

void renderEnd(struct GraphicsTask* graphicsTask, struct GameConfig* gameConfig);

#endif
