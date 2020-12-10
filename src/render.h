#ifndef RENDER_H
#define RENDER_H

struct ProgramState;
struct GameState;
struct GameConfig;
struct GraphicsTask;

float menuRotation;
float menuFloating;

void render(struct ProgramState* programState);

void renderGame(struct GraphicsTask* graphicsTask, struct GameState* gameState);

void renderMenu(struct GraphicsTask* graphicsTask, struct GameConfig* gameConfig, struct GameState* gameState);

void renderEnd(struct GraphicsTask* graphicsTask, struct GameState* gameState);

#endif
