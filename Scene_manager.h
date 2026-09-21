#pragma once
#include <stdbool.h>
#include "Game_mechanics.h"
#include "Game_master.h"

//Display Definitions
#define SLOT_DISPLAY_OFFSET 1   // slots are 0-3 inside, shown as 1-4

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
void renderMonsterTitle(int monsterValue);
const char* cardTypeName(Card* card);
const char* cardActionVerb(Card* card);
const char* cardValueLabel(Card* card);

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
void renderWeaponSwapConfirm(Player* player);
void renderPotionWasteConfirm(void);
bool promptConfirm(Game* session, EncounterPrompt prompt, int chosenSlot);

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