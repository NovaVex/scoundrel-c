#pragma once
#include <stdbool.h>
#include "Data_Structure.h"

// ========================================================
// CORE SCENES
// ========================================================
int openMainMenu();
int runActiveGameScene(struct game* session, bool debugViewEnabled);
int runPauseScene();
int runOptionsScene(bool debugViewEnabled);
bool askFightWithWeapon(int weaponValue, int monsterValue);
void runGameOverScene(int finalScore);
void runVictoryScene(int finalScore);

// ========================================================
// DEBUG SCENES
// ========================================================
void debugGenerateTempTestDeck();
void openDebugMenu(struct gameMaster* engine, struct game* activeSession);
