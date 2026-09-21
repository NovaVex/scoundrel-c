#pragma once
#include <stdbool.h>
#include "Game_mechanics.h"

//Version Definitions
#define GAME_VERSION "0.1.2"

//Game settings definitions Maybe move definitions.
#define AUTO_RESOLVE_COMBAT_DEFAULT false
#define AUTO_CONFIRM_WEAPON_SWAP_DEFAULT false

//Debug Definitions
#define DEBUG_MODE_DEFAULT false
#define RIGGED_DECK_DEFAULT false

//RNG Definitions
#define RNG_WARMUP_MAX_SKIPS 100

typedef enum SettingID {
    SETTING_DEBUG_MODE,
    SETTING_AUTO_RESOLVE_COMBAT,
    SETTING_AUTO_CONFIRM_WEAPON_SWAP,
    SETTING_RIGGED_DECK,
    SETTINGS_COUNT
} SettingID;

typedef struct GameSettings {
    bool toggles[SETTINGS_COUNT];
} GameSettings;

typedef enum SystemState {
    SYSTEM_EXIT = 0,
    SYSTEM_MAIN_MENU = 1,
    SYSTEM_IN_GAME = 2,
    SYSTEM_OPTIONS = 3
} SystemState;

typedef enum InGameState {
    PLAYING_ACTIVE,
    PLAYING_PAUSED,
    PLAYING_GAMEOVER,
    PLAYING_EXIT
} InGameState;

typedef struct GameMaster {
    SystemState systemState;
    GameSettings settings;
    bool isDebugMenuOpen;
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
void setGameSettingsDefault(GameMaster* gm);
void gameSetUp(Game* session, bool useRiggedDeck);
void randomNumberGenerator(GameMaster* gm);

// ========================================================
// System routing
// ========================================================
SystemState mainMenuManager(GameMaster* gm);
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
InGameState activeGameOverManager(Game* session, GameMaster* gm);

// ========================================================
// Settings
// ========================================================
bool isSettingOn(GameMaster* gm, SettingID setting);
void setSetting(GameMaster* gm, SettingID setting, bool state);
void toggleSetting(GameMaster* gm, SettingID setting);
