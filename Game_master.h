#pragma once

void gameMaster();

void wakeGameMaster(struct gameMaster* startingValues);

void gameLoop(struct gameMaster* gm);

void gameSetUp(struct game* session);

void setPlayerDefault(struct player* defaultValues);

bool isGameSessionActive(struct game* session);

void randomNumberGenerator(struct gameMaster* engine);