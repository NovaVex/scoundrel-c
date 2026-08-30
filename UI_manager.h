#pragma once
#include <stdbool.h>
#include "Game_mechanics.h"
#include "Game_master.h"

// ========================================================
// Basic utilities
// ========================================================
void initializeDisplay(void);
void clearScreen(void);

// ========================================================
// Screen furniture
// ========================================================
void renderRule(void);
void renderSeparator(void);
void renderTitle(const char* title);
void renderMenuOption(int optionKey, const char* label);
void renderToggleOption(int optionKey, const char* label, bool isEnabled);
void renderPrompt(const char* label);

// ========================================================
// Menu rendering
// ========================================================
void renderMainMenu(GameMaster* gm);
void renderPauseMenu(GameMaster* gm);
void renderOptionsMenu(GameMaster* gm);
void renderDebugLine(GameMaster* gm, int optionKey);
void renderDebugMenu(void);
void renderTurnComplete(void);
void renderGameOver(bool playerDied, int finalScore);

// ========================================================
// Play rendering
// ========================================================
void renderGameState(Game* session);
void renderWeaponLine(Player* player);
void renderRoomSlots(Game* session);
void renderActionMenu(Game* session);

// ========================================================
// Messages
// ========================================================
void renderUnknownCommand(void);
void renderNoActiveSession(void);
void renderEmptySlotChosen(void);
void renderRoomNotCleared(void);
void renderFleeBlocked(void);
void renderInvalidSelection(void);

// ========================================================
// Action prompts
// ========================================================
void renderCombatChoicePrompt(Game* session, int chosenSlot);
void renderBareHandedConfirm(Game* session, int chosenSlot);
void renderWeaponSwapConfirm(int cardsAtRisk);
void renderPotionWasteConfirm(void);

// ========================================================
// Debug printing (targeted)
// ========================================================
void printCurrentPlayerStats(Game* session);
void printEntireDeckLoop(Zone* pile);

// ========================================================
// Debug tools (session level)
// ========================================================
void printSessionDeck(Game* session);
void printDiscardPile(Game* session);
void printDungeonRoom(Game* session);
