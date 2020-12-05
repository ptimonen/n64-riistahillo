#ifndef GAME_SETUP_H
#define GAME_SETUP_H

struct GameState;
struct GameConfig;
struct ProgramState;

void setupGameState(struct GameState* gameState, struct GameConfig* gameConfig);
void setupProgramState(struct ProgramState* programState);

#endif