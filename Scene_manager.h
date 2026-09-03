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
void renderDebugLine(GameMaster* gm, int optionKey);

// ========================================================
// Messages
// ========================================================
void renderNoActiveSession(void);
void renderEmptySlotChosen(void);
void renderRoomNotCleared(void);
void renderFleeBlocked(void);
void renderInvalidSelection(void);

// ========================================================
// Menu screens
// ========================================================
void renderMainMenu(GameMaster* gm);
int openMainMenu(GameMaster* gm);
void renderPauseMenu(GameMaster* gm);
int openPauseScene(GameMaster* gm);
void renderOptionsMenu(GameMaster* gm);
int openOptionsScene(GameMaster* gm);

// ========================================================
// Play screens
// ========================================================
void renderGameState(Game* session);
void renderWeaponLine(Player* player);
void renderRoomSlots(Game* session);
void renderActionMenu(Game* session);
int runActiveGameScene(Game* session);
void renderTurnComplete(void);
void runTurnCompleteScene(Game* session);
void renderGameOver(bool playerDied, int finalScore);
void openGameOverScene(bool playerDied, int finalScore);

// ========================================================
// Action prompts
// ========================================================
void renderCombatChoicePrompt(Game* session, int chosenSlot);
CombatChoice promptCombatChoice(Game* session, int chosenSlot);
void renderBareHandedConfirm(Game* session, int chosenSlot);
bool promptBareHandedConfirm(Game* session, int chosenSlot);
void renderWeaponSwapConfirm(int cardsAtRisk);
bool promptWeaponSwapConfirm(Game* session);
void renderPotionWasteConfirm(void);
bool promptPotionWasteConfirm(Game* session);

// ========================================================
// Turn flow
// ========================================================
EncounterResult runEncounterScene(Game* session, GameMaster* gm, int chosenSlot);
FleeResult runFleeScene(Game* session);
void reportEncounterResult(EncounterResult result);

// ========================================================
// Debug screens
// ========================================================
void renderDebugMenu(void);
void openDebugMenu(GameMaster* gm, Game* session);
void debugGenerateTempTestDeck(void);
void printCurrentPlayerStats(Game* session);
void printEntireDeckLoop(Zone* pile);
void printSessionDeck(Game* session);
void printDiscardPile(Game* session);
void printDungeonRoom(Game* session);