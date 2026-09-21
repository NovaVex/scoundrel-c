// ========================================================
// Game_master.c
// --------------------------------------------------------
// The conductor: loops, state machines and settings.
// No rules and no printing.
//
// gameMaster()  outer loop: main menu / options / in game
// gameLoop()    inner loop: active / paused / game over
// Each loop calls the manager for its state, which returns the next state.
// ========================================================

#include "Game_master.h"
#include "Terminal_Input.h"
#include "Scene_manager.h"
#include "Game_mechanics.h"
#include <stdlib.h>
#include <time.h>

// ========================================================
// System entry
// ========================================================
int gameMaster(void) {
    GameMaster gm = {0};

    wakeGameMaster(&gm);

    while (gm.systemState != SYSTEM_EXIT) {
        switch (gm.systemState) {
            case SYSTEM_MAIN_MENU:
                gm.systemState = mainMenuManager(&gm);
                break;

            case SYSTEM_IN_GAME:
                gameLoop(&gm);
                break;

            case SYSTEM_OPTIONS:
                optionsLoop(&gm);
                gm.systemState = SYSTEM_MAIN_MENU;
                break;

            case SYSTEM_EXIT:
            default:
                gm.systemState = SYSTEM_EXIT;
                break;
        }
    }

    return 0;
}

// ========================================================
// Initialization
// ========================================================
void wakeGameMaster(GameMaster* gm) {
    setGameSettingsDefault(gm);

    gm->isDebugMenuOpen = false;
    gm->systemState     = SYSTEM_MAIN_MENU;

    initializeDisplay();
    randomNumberGenerator(gm);
}

//Need to work on.
void setGameSettingsDefault(GameMaster* gm) {
    setSetting(gm, SETTING_DEBUG_MODE, DEBUG_MODE_DEFAULT);
    setSetting(gm, SETTING_AUTO_RESOLVE_COMBAT, AUTO_RESOLVE_COMBAT_DEFAULT);
    setSetting(gm, SETTING_AUTO_CONFIRM_WEAPON_SWAP, AUTO_CONFIRM_WEAPON_SWAP_DEFAULT);
    setSetting(gm, SETTING_RIGGED_DECK, RIGGED_DECK_DEFAULT);
}

// The rigged deck is never shuffled.
void gameSetUp(Game* session, bool useRiggedDeck) {
    setPlayerDefault(&session->playerOne);

    // The rigged pool if useRiggedDeck is on, otherwise the normal pool
    int totalCards = useRiggedDeck ? generateRiggedCardPool(session->globalCardPool)
                                   : generateGlobalCardPool(session->globalCardPool);

    buildDeck(session, totalCards);

    if (!useRiggedDeck) {   // If this is a normal deck
        cardShuffle(session->mainDeck.cards, totalCards);
    }

    dealRoomCards(session);
}

void randomNumberGenerator(GameMaster* gm) {
    unsigned int startingSeed = (unsigned int)time(NULL);

    srand(startingSeed);

    int rngToSkip = rand() % RNG_WARMUP_MAX_SKIPS;   // random number from 0 up to RNG_WARMUP_MAX_SKIPS - 1

    for (int rngIncrementer = 0; rngIncrementer < rngToSkip; rngIncrementer++) {
        rand();
    }

    gm->rngSeed = startingSeed;
}

// ========================================================
// System routing
// ========================================================
SystemState mainMenuManager(GameMaster* gm) {
    int playerChoice = openMainMenu(gm);

    switch (playerChoice) {
        case MENU_START_GAME:
            return SYSTEM_IN_GAME;

        case MENU_OPTIONS:
            return SYSTEM_OPTIONS;

        case MENU_DEBUG:
            openDebugMenu(gm, NULL);
            break;

        case INPUT_DEBUG_COMMAND:
            toggleSetting(gm, SETTING_DEBUG_MODE);
            break;

        case MENU_QUIT:
        case INPUT_END_OF_STREAM:
            return SYSTEM_EXIT;

        default:
            break;
    }

    return SYSTEM_MAIN_MENU;
}

//TODO: not quiet sure what the AI was on. Possible Clean up.
void applyOptionsToggle(GameMaster* gm, int playerChoice) {
    switch (playerChoice) {
        // Off only. Typing "debug" at the main menu is the only way on.
        case OPTIONS_TOGGLE_DEBUG:
            setSetting(gm, SETTING_DEBUG_MODE, false);
            break;

        case OPTIONS_TOGGLE_AUTO_COMBAT:
            toggleSetting(gm, SETTING_AUTO_RESOLVE_COMBAT);
            break;

        case OPTIONS_TOGGLE_AUTO_EQUIP:
            toggleSetting(gm, SETTING_AUTO_CONFIRM_WEAPON_SWAP);
            break;

        // Hidden with the debug line, so it does nothing unless debug is on.
        case OPTIONS_TOGGLE_RIGGED_DECK:
            if (!isSettingOn(gm, SETTING_DEBUG_MODE)) break;   // If debug mode is off, do nothing

            toggleSetting(gm, SETTING_RIGGED_DECK);
            break;

        default:
            break;
    }
}

void optionsLoop(GameMaster* gm) {
    while (true) {
        int playerChoice = openOptionsScene(gm);

        if (playerChoice == OPTIONS_BACK) return;
        if (playerChoice == INPUT_END_OF_STREAM) return;

        applyOptionsToggle(gm, playerChoice);
    }
}

// ========================================================
// Game loop managers
// ========================================================
void gameLoop(GameMaster* gm) {
    Game session = {0};
    InGameState currentGameState = PLAYING_ACTIVE;

    // Turning debug off also turns the rigged deck off.
    // && means AND: debug mode and the rigged deck must both be on
    bool useRiggedDeck = isSettingOn(gm, SETTING_DEBUG_MODE) && isSettingOn(gm, SETTING_RIGGED_DECK);

    gameSetUp(&session, useRiggedDeck);

    while (currentGameState != PLAYING_EXIT) {
        switch (currentGameState) {
            case PLAYING_ACTIVE:
                currentGameState = activeGameManager(&session, gm);
                break;

            case PLAYING_PAUSED:
                currentGameState = activeGamePauseManager(&session, gm);
                break;

            case PLAYING_GAMEOVER:
                currentGameState = activeGameOverManager(&session, gm);
                break;

            case PLAYING_EXIT:
            default:
                currentGameState = PLAYING_EXIT;
                break;
        }
    }

    gm->systemState = SYSTEM_MAIN_MENU;
}

// ========================================================
// State sub-managers
// ========================================================
InGameState activeGameManager(Game* session, GameMaster* gm) {
    if (isGameOver(session)) return PLAYING_GAMEOVER;

    if (isRoomComplete(session)) {
        runTurnCompleteScene(session);
        advanceToNextRoom(session, PLAYER_DEFAULT_FLEE_STATE);
        return PLAYING_ACTIVE;
    }

    int playerChoice = runActiveGameScene(session);

    switch (playerChoice) {
        case INPUT_SLOT_0:
        case INPUT_SLOT_1:
        case INPUT_SLOT_2:
        case INPUT_SLOT_3:
            // Keys 1-4 map to slots 0-3. See renderActionMenu.
            runEncounterScene(session, gm, playerChoice - INPUT_SLOT_0);
            break;

        case INPUT_FLEE:
            runFleeScene(session);
            break;

        case INPUT_PAUSE:
            return PLAYING_PAUSED;

        case INPUT_END_OF_STREAM:
            return PLAYING_EXIT;

        default:
            break;
    }

    return PLAYING_ACTIVE;
}

InGameState activeGamePauseManager(Game* session, GameMaster* gm) {
    int playerChoice = openPauseScene(gm);

    switch (playerChoice) {
        case PAUSE_RESUME:
            return PLAYING_ACTIVE;

        case PAUSE_OPTIONS:
            optionsLoop(gm);
            return PLAYING_PAUSED;

        case PAUSE_DEBUG:
            openDebugMenu(gm, session);
            break;

        case PAUSE_QUIT:
        case INPUT_END_OF_STREAM:
            return PLAYING_EXIT;

        default:
            break;
    }

    return PLAYING_PAUSED;
}

InGameState activeGameOverManager(Game* session, GameMaster* gm) {
    (void)gm;   // tells the compiler gm is unused on purpose

    bool playerDied = isPlayerDead(&session->playerOne);
    int finalScore = calculateFinalScore(session);

    openGameOverScene(playerDied, finalScore);

    return PLAYING_EXIT;
}

// ========================================================
// Settings
// ========================================================
bool isSettingOn(GameMaster* gm, SettingID setting) {
    return gm->settings.toggles[setting];
}

void setSetting(GameMaster* gm, SettingID setting, bool state) {
    gm->settings.toggles[setting] = state;
}

void toggleSetting(GameMaster* gm, SettingID setting) {
    bool flippedState = !isSettingOn(gm, setting);   // the opposite of what it is now

    setSetting(gm, setting, flippedState);
}
