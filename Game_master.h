#pragma once
#include <stdbool.h>
#include "Data_Structure.h"

// ========================================================
// System entry
// ========================================================
int gameMaster(void);

// ========================================================
// Initialization
// ========================================================
void wakeGameMaster(GameMaster* gm);
void gameSetUp(Game* session);
void randomNumberGenerator(GameMaster* gm);

// ========================================================
// System routing
// ========================================================
void routeMainMenuChoice(GameMaster* gm, int playerChoice);
void applyOptionsToggle(GameMaster* gm, int playerChoice);
void optionsLoop(GameMaster* gm);

// ========================================================
// Game loop managers
// ========================================================
void gameLoop(GameMaster* gm);

// ========================================================
// State sub-managers
// ========================================================
InGameState activeGameManager(Game* session, GameMaster* gm);
InGameState activeGamePauseManager(Game* session, GameMaster* gm);
InGameState activeGameOptionsManager(Game* session, GameMaster* gm);
InGameState activeGameOverManager(Game* session, GameMaster* gm);
