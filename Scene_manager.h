#pragma once
#include <stdbool.h>
#include "Data_Structure.h"

// ========================================================
// CORE SCENES
// ========================================================
int openMainMenu(GameMaster* gm);
int runActiveGameScene(Game* session);
int openPauseScene(GameMaster* gm);
int openOptionsScene(GameMaster* gm);
void openGameOverScene(bool playerDied, int finalScore);

// ========================================================
// DEBUG SCENES
// ========================================================
void debugGenerateTempTestDeck(void);
void openDebugMenu(GameMaster* gm, Game* session);

// ========================================================
// ACTION SCENES
// ========================================================
EncounterResult runEncounterScene(Game* session, GameMaster* gm, int chosenSlot);
void runTurnCompleteScene(Game* session);
FleeResult runFleeScene(Game* session);
void reportEncounterResult(EncounterResult result);
CombatChoice promptCombatChoice(Game* session, int chosenSlot);
bool promptBareHandedConfirm(Game* session, int chosenSlot);
bool promptWeaponSwapConfirm(Game* session);
bool promptPotionWasteConfirm(Game* session);
