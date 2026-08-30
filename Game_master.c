// ========================================================
// Game_master.c
// --------------------------------------------------------
// JOB
//   The conductor. It owns the loops and decides which screen
//   the player is looking at. It holds no rules and prints
//   nothing itself.
//
// THE TWO STATE MACHINES
//   gameMaster()  outer loop: main menu / options / in game
//   gameLoop()    inner loop: active / paused / options / game over
//
//   Each loop just asks "what state am I in?", calls the one
//   manager for that state, and that manager returns the next
//   state. Nothing else happens in the loops.
//
// ONE TURN, START TO FINISH
//   Follow this once and the whole program makes sense:
//
//   1. activeGameManager() asks Game_mechanics: is the game over?
//   2. ...and: is the room finished? (isRoomComplete)
//         if yes -> show the turn-complete screen, deal the next
//                   room, done. The player is never asked.
//   3. otherwise -> runActiveGameScene() draws the room and
//                   returns whatever key the player pressed
//   4. the switch below turns that key into one action:
//         slot 1-4 -> runEncounterScene()   (Scene_manager.c)
//         5        -> runFleeScene()        (Scene_manager.c)
//         9        -> pause
//   5. the scene asks any needed question, then hands the answer
//      to Game_mechanics.c, which changes the game state
//   6. back to step 1
//
// WHAT THIS FILE CALLS
//   Scene_manager.h    to show screens and collect choices
//   Game_mechanics.h   to ask about game state and apply turns
// ========================================================

#include "Game_master.h"
#include "Terminal_Input.h"
#include "Scene_manager.h"
#include "Game_mechanics.h"
#include "UI_manager.h"
#include <stdlib.h>
#include <time.h>

// ========================================================
// System entry
// ========================================================
int gameMaster(void) {
    GameMaster gm = {0};

    wakeGameMaster(&gm);

    while (gm.gameState != SYSTEM_EXIT) {
        switch (gm.gameState) {
            case SYSTEM_MAIN_MENU:
                routeMainMenuChoice(&gm, openMainMenu(&gm));
                break;

            case SYSTEM_IN_GAME:
                gameLoop(&gm);
                break;

            case SYSTEM_OPTIONS:
                optionsLoop(&gm);
                gm.gameState = SYSTEM_MAIN_MENU;
                break;

            case SYSTEM_EXIT:
            default:
                gm.gameState = SYSTEM_EXIT;
                break;
        }
    }

    return 0;
}

// ========================================================
// Initialization
// ========================================================
void wakeGameMaster(GameMaster* gm) {
    gm->debugMenuEnabled = false;
    gm->debugOpen = false;
    gm->autoResolveCombat = false;
    gm->autoConfirmWeaponSwap = false;
    gm->gameState = SYSTEM_MAIN_MENU;

    initializeDisplay();
    randomNumberGenerator(gm);
}

void gameSetUp(Game* session) {
    int totalCards = 0;

    setPlayerDefault(&session->playerOne);
    generateCardPool(session, &totalCards);
    buildDeck(session, totalCards);

    dealRoomCards(session);
}

void randomNumberGenerator(GameMaster* gm) {
    unsigned int startingSeed = (unsigned int)time(NULL);

    srand(startingSeed);

    int rngToSkip = rand() % 100;

    for (int rngIncrementer = 0; rngIncrementer < rngToSkip; rngIncrementer++) {
        rand();
    }

    gm->rngSeed = startingSeed;
}

// ========================================================
// System routing
// ========================================================
void routeMainMenuChoice(GameMaster* gm, int playerChoice) {
    switch (playerChoice) {
        case MENU_START_GAME:
            gm->gameState = SYSTEM_IN_GAME;
            break;

        case MENU_OPTIONS:
            gm->gameState = SYSTEM_OPTIONS;
            break;

        case MENU_DEBUG:
            openDebugMenu(gm, NULL);
            break;

        case INPUT_DEBUG_COMMAND:
            gm->debugMenuEnabled = !gm->debugMenuEnabled;
            break;

        case MENU_QUIT:
        case INPUT_END_OF_STREAM:
            gm->gameState = SYSTEM_EXIT;
            break;

        default:
            break;
    }
}

void applyOptionsToggle(GameMaster* gm, int playerChoice) {
    switch (playerChoice) {
        // One-way switch by design. The options screen only draws this
        // line while the egg is already armed, so its job is to put the
        // tools away again. Typing "debug" at the main menu stays the
        // only way to arm them, and pressing 1 on a hidden line does
        // nothing rather than quietly handing them over.
        case OPTIONS_TOGGLE_DEBUG:
            if (!gm->debugMenuEnabled) break;

            gm->debugMenuEnabled = false;
            break;

        case OPTIONS_TOGGLE_AUTO_COMBAT:
            gm->autoResolveCombat = !gm->autoResolveCombat;
            break;

        case OPTIONS_TOGGLE_AUTO_EQUIP:
            gm->autoConfirmWeaponSwap = !gm->autoConfirmWeaponSwap;
            break;

        default:
            break;
    }
}

void optionsLoop(GameMaster* gm) {
    bool optionsOpen = true;

    while (optionsOpen) {
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

    gameSetUp(&session);

    while (currentGameState != PLAYING_EXIT) {
        switch (currentGameState) {
            case PLAYING_ACTIVE:
                currentGameState = activeGameManager(&session, gm);
                break;

            case PLAYING_PAUSED:
                currentGameState = activeGamePauseManager(&session, gm);
                break;

            case PLAYING_OPTIONS:
                currentGameState = activeGameOptionsManager(&session, gm);
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

    gm->gameState = SYSTEM_MAIN_MENU;
}

// ========================================================
// State sub-managers
// ========================================================
InGameState activeGameManager(Game* session, GameMaster* gm) {
    if (isGameOver(session)) return PLAYING_GAMEOVER;

    if (isRoomComplete(session)) {
        runTurnCompleteScene(session);
        advanceToNextRoom(session);
        return PLAYING_ACTIVE;
    }

    int playerChoice = runActiveGameScene(session);

    switch (playerChoice) {
        case INPUT_SLOT_0:
        case INPUT_SLOT_1:
        case INPUT_SLOT_2:
        case INPUT_SLOT_3:
            runEncounterScene(session, gm, playerChoice - 1);
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
            return PLAYING_OPTIONS;

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

InGameState activeGameOptionsManager(Game* session, GameMaster* gm) {
    (void)session;

    int playerChoice = openOptionsScene(gm);

    if (playerChoice == OPTIONS_BACK) return PLAYING_PAUSED;
    if (playerChoice == INPUT_END_OF_STREAM) return PLAYING_EXIT;

    applyOptionsToggle(gm, playerChoice);

    return PLAYING_OPTIONS;
}

InGameState activeGameOverManager(Game* session, GameMaster* gm) {
    (void)gm;

    bool playerDied = isPlayerDead(&session->playerOne);
    int finalScore = calculateFinalScore(session);

    openGameOverScene(playerDied, finalScore);

    return PLAYING_EXIT;
}
