#ifndef INPUT_H
#define INPUT_H

struct GameState;
struct Player;
struct ProgramState;

void updateGameInput(struct GameState* gameState);
void updatePlayerInput(struct Player* player);
void updateMenuInput(struct ProgramState* programState);

#endif
