#pragma once
#include "Data_Structure.h"

// ========================================================
// Basic utilities
// ========================================================
void clearScreen();

// ========================================================
// Debug printing (targeted)
// ========================================================
void printCurrentPlayerStats(struct game* activeSession);
void printEntireDeckLoop(Zone* pile);

// ========================================================
// Debug tools (session level)
// ========================================================
void printSessionDeck(struct game* activeSession);
void printDungeonRoom(struct game* activeSession);

// ========================================================
// Master debug renderer
// ========================================================
void debugRender(struct game* game);