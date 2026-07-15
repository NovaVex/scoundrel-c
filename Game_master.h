#pragma once
#include <stdbool.h>
#include "Data_Structure.h"
#include "Game_master.h"

// ========================================================
// System entry
// ========================================================
int gameMaster();

// ========================================================
// Initialization
// ========================================================
void wakeGameMaster(struct gameMaster* startingValues);
void gameSetUp(struct game* session);
void setPlayerDefault(struct player* playerOne);
void randomNumberGenerator(struct gameMaster* engine);

// ========================================================
// Game loop managers
// ========================================================
void gameLoop(struct gameMaster* gm);

// ========================================================
// State sub-managers
// ========================================================
InGameState activeGameManger(struct game* session, struct gameMaster* gm);
InGameState activeGamePauseManger(struct game* session, struct gameMaster* gm);
InGameState activeGameOptionsManger(struct game* session, struct gameMaster* gm);
InGameState activeGameOverManger(struct game* session, struct gameMaster* gm);

// ========================================================
// Helpers
// ========================================================
bool isGameSessionActive(struct game* session);
