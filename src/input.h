#ifndef INPUT_H
#define INPUT_H

struct GameState;
struct ProgramState;

void updateGameInput(struct GameState* gameState);
void updatePlayerInput(struct GameState* gameState, int playerIndex);
void updateMenuInput(struct ProgramState* programState);

#endif
