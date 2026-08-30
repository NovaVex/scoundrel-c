#pragma once
#include <stdbool.h>
#include "Game_mechanics.h"

#define GAME_VERSION "0.1.1"

// ========================================================
// State machines
// ========================================================
typedef enum SystemState {
    SYSTEM_EXIT = 0,
    SYSTEM_MAIN_MENU = 1,
    SYSTEM_IN_GAME = 2,
    SYSTEM_OPTIONS = 3
} SystemState;

typedef enum InGameState {
    PLAYING_ACTIVE,
    PLAYING_PAUSED,
    PLAYING_OPTIONS,
    PLAYING_GAMEOVER,
    PLAYING_EXIT
} InGameState;

typedef struct GameMaster {
    bool debugMenuEnabled;
    bool debugOpen;
    bool autoResolveCombat;
    bool autoConfirmWeaponSwap;
    SystemState gameState;
    unsigned int rngSeed;
} GameMaster;


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
