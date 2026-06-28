#pragma once
#include <stdbool.h>
#include "Data_Structure.h"

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
void seedRandomNumberGenerator(struct gameMaster* engine);

// ========================================================
// Game loop managers
// ========================================================
void gameLoop(struct gameMaster* engine);

// ========================================================
// State sub-managers
// ========================================================
InGameState activeGameManager(struct game* session, struct gameMaster* engine);
InGameState activeGamePauseManager();
InGameState activeGameOptionsManager(struct gameMaster* engine);
InGameState activeGameOverManager(struct game* session);
InGameState activeGameVictoryManager(struct game* session);

// ========================================================
// Player action handlers
// ========================================================
InGameState routePlayerAction(struct game* session, int playerChoice);
void handleSlotChoice(struct game* session, int slotIndex);
void handleNextRoomChoice(struct game* session);
bool resolveFightBarehanded(struct game* session, int slotIndex);
