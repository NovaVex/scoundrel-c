#pragma once
#include <stdbool.h>
#include "Data_Structure.h"

// ========================================================
// Basic utilities
// ========================================================
void clearScreen();

// ========================================================
// Player-facing rendering
// ========================================================
void renderGameView(struct game* session);
void printHealthLine(struct player* activePlayer, int cardsInDeck);
void printWeaponLine(struct weapon* playerWeapon);
void printRoomSlots(struct game* session);

// ========================================================
// Player-facing messages
// ========================================================
void printEncounterMessage(EncounterResult result);
void printLastCardWarning();
void printGameOverScreen(int finalScore);
void printVictoryScreen(int finalScore);

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
void debugRender(struct game* session);
