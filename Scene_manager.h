#pragma once
#include "Data_Structure.h"

// ========================================================
// CORE SCENES
// ========================================================
int openMainMenu();
int runActiveGameScene(struct game* session);

// ========================================================
// DEBUG SCENES
// ========================================================
void debugGenerateTempTestDeck();
void openDebugMenu(struct gameMaster* engine, struct game* activeSession);