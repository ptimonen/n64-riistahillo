#ifndef INPUT_H
#define INPUT_H

struct GameState;
struct Player;
struct ProgramState;

void updateGameInput(struct ProgramState* programState);
void updatePlayerInput(struct ProgramState* programState, struct Player* player);
void updateMenuInput(struct ProgramState* programState);
void updateEndInput(struct ProgramState* programState);

#endif
