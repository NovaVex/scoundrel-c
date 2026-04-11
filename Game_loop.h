#pragma once

void gameLoop(struct gameMaster* gm);
void randomNumberGenerator(struct gameMaster* engine);
bool isGameSessionActive(struct game* session);
void generateDeck(struct deck* generate);
void setPlayerDefault(struct player*, defaultValues);
void shuffleDeck(int deck[], int deckSize);
void gameSetUp(struct game* session);
void printDungeonRoom(struct game* activeSession);
void openDebugMenu(struct gameMaster* engine, struct game* activeSession);