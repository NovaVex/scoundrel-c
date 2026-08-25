// ========================================================
// Scene_manager.c
// --------------------------------------------------------
// JOB
//   A "scene" is one screen the player looks at. Each function
//   here does the same three steps in the same order:
//
//       1. clearScreen()          wipe the terminal
//       2. render...()            draw it   (UI_manager.c)
//       3. processUserInput()     read them (Input.c)
//
//   That is the entire pattern. Once you see it, every
//   function in this file reads the same way.
//
// TEMPORARY
//   Throwaway layer, same as UI_manager.c. The engine port
//   replaces it.
//
// WHO CALLS THIS FILE
//   Game_master.c, and nothing else.
//
// WHAT THIS FILE CALLS
//   UI_manager.h       to draw
//   Input.h            to read the keyboard
//   Game_mechanics.h   to ask what is legal, and to apply the
//                      player's choice once it is made
//
// THE ONE RULE
//   This file decides HOW to ask a question. It never decides
//   WHAT the rules are. Every rule question goes to
//   Game_mechanics.c and comes back as a plain answer.
//
// MAP OF THIS FILE, IN ORDER
//   CORE SCENES     the menus and the main play screen
//   ACTION SCENES   encounters, fleeing, end of turn
//   DEBUG SCENES    developer tools
// ========================================================

#include "Scene_manager.h"
#include "Data_Structure.h"
#include "Game_mechanics.h"
#include "UI_manager.h"
#include "Input.h"
#include <stdbool.h>

// ========================================================
// CORE SCENES
// ========================================================
int openMainMenu(GameMaster* gm) {
    clearScreen();
    renderMainMenu(gm);

    return processUserInput();
}

int runActiveGameScene(Game* session) {
    clearScreen();
    renderGameState(session);
    renderActionMenu(session);

    return processUserInput();
}

int openPauseScene(GameMaster* gm) {
    clearScreen();
    renderPauseMenu(gm);

    return processUserInput();
}

int openOptionsScene(GameMaster* gm) {
    clearScreen();
    renderOptionsMenu(gm);

    return processUserInput();
}

void openGameOverScene(bool playerDied, int finalScore) {
    clearScreen();
    renderGameOver(playerDied, finalScore);
    pressEnterToContinue();
}

// ========================================================
// ACTION SCENES
// ========================================================
EncounterResult runEncounterScene(Game* session, GameMaster* gm, int chosenSlot) {
    CombatChoice combatChoice = COMBAT_CHOICE_BARE_HANDED;

    switch (requiredEncounterPrompt(session, chosenSlot)) {
        case ENCOUNTER_PROMPT_COMBAT_CHOICE:
            if (gm->autoResolveCombat) {
                combatChoice = COMBAT_CHOICE_USE_WEAPON;
                break;
            }
            combatChoice = promptCombatChoice(session, chosenSlot);
            break;

        case ENCOUNTER_PROMPT_BARE_HANDED_CONFIRM:
            if (gm->autoResolveCombat) break;

            if (!promptBareHandedConfirm(session, chosenSlot)) {
                return ENCOUNTER_CANCELLED;
            }
            break;

        case ENCOUNTER_PROMPT_WEAPON_SWAP:
            if (gm->autoConfirmWeaponSwap) break;

            if (!promptWeaponSwapConfirm(pendingWeaponDiscardCount(&session->playerOne))) {
                return ENCOUNTER_CANCELLED;
            }
            break;

        case ENCOUNTER_PROMPT_POTION_WASTE:
            if (!promptPotionWasteConfirm()) return ENCOUNTER_CANCELLED;
            break;

        case ENCOUNTER_PROMPT_NONE:
        default:
            break;
    }

    EncounterResult result = encounterManager(session, chosenSlot, combatChoice);

    reportEncounterResult(result);

    return result;
}

void runTurnCompleteScene(Game* session) {
    clearScreen();
    renderGameState(session);
    renderTurnComplete();
    pressEnterToContinue();
}

FleeResult runFleeScene(Game* session) {
    FleeResult result = fleeManager(session);

    if (result == FLEE_BLOCKED) {
        renderFleeBlocked();
        pressEnterToContinue();
    }

    return result;
}

void reportEncounterResult(EncounterResult result) {
    switch (result) {
        case ENCOUNTER_BLOCKED_EMPTY_SLOT:
            renderEmptySlotChosen();
            pressEnterToContinue();
            break;

        case ENCOUNTER_BLOCKED_ROOM_NOT_CLEARED:
            renderRoomNotCleared();
            pressEnterToContinue();
            break;

        case ENCOUNTER_RESOLVED:
        case ENCOUNTER_CANCELLED:
        default:
            break;
    }
}

CombatChoice promptCombatChoice(Game* session, int chosenSlot) {
    while (true) {
        clearScreen();
        renderCombatChoicePrompt(session, chosenSlot);

        int playerChoice = processUserInput();

        if (playerChoice == COMBAT_CHOICE_USE_WEAPON) return COMBAT_CHOICE_USE_WEAPON;
        if (playerChoice == COMBAT_CHOICE_BARE_HANDED) return COMBAT_CHOICE_BARE_HANDED;
        if (playerChoice == INPUT_END_OF_STREAM) return COMBAT_CHOICE_BARE_HANDED;

        renderInvalidSelection();
        pressEnterToContinue();
    }
}

bool promptBareHandedConfirm(Game* session, int chosenSlot) {
    while (true) {
        clearScreen();
        renderGameState(session);
        renderBareHandedConfirm(session, chosenSlot);

        int playerChoice = processUserInput();

        if (playerChoice == CONFIRM_YES) return true;
        if (playerChoice == CONFIRM_NO) return false;
        if (playerChoice == INPUT_END_OF_STREAM) return false;

        renderInvalidSelection();
        pressEnterToContinue();
    }
}

bool promptWeaponSwapConfirm(int cardsAtRisk) {
    while (true) {
        clearScreen();
        renderWeaponSwapConfirm(cardsAtRisk);

        int playerChoice = processUserInput();

        if (playerChoice == CONFIRM_YES) return true;
        if (playerChoice == CONFIRM_NO) return false;
        if (playerChoice == INPUT_END_OF_STREAM) return false;

        renderInvalidSelection();
        pressEnterToContinue();
    }
}

bool promptPotionWasteConfirm(void) {
    while (true) {
        clearScreen();
        renderPotionWasteConfirm();

        int playerChoice = processUserInput();

        if (playerChoice == CONFIRM_YES) return true;
        if (playerChoice == CONFIRM_NO) return false;
        if (playerChoice == INPUT_END_OF_STREAM) return false;

        renderInvalidSelection();
        pressEnterToContinue();
    }
}

// ========================================================
// DEBUG SCENES
// ========================================================
void debugGenerateTempTestDeck(void) {
    Game tempSession = {0};
    int totalCards = 0;

    generateCardPool(&tempSession, &totalCards);
    buildDeck(&tempSession, totalCards);

    printEntireDeckLoop(&tempSession.mainDeck);
    pressEnterToContinue();
}

void openDebugMenu(GameMaster* gm, Game* session) {
    if (!gm->debugMenuEnabled) return;

    gm->debugOpen = true;

    while (gm->debugOpen) {
        clearScreen();
        renderDebugMenu();

        int playerChoice = processUserInput();

        switch (playerChoice) {
            case DEBUG_PRINT_MAIN_DECK:
                printSessionDeck(session);
                pressEnterToContinue();
                break;

            case DEBUG_PRINT_DISCARD_PILE:
                printDiscardPile(session);
                pressEnterToContinue();
                break;

            case DEBUG_PRINT_TEMP_DECK:
                debugGenerateTempTestDeck();
                break;

            case DEBUG_PRINT_PLAYER_STATS:
                printCurrentPlayerStats(session);
                pressEnterToContinue();
                break;

            case DEBUG_PRINT_DUNGEON_ROOM:
                printDungeonRoom(session);
                pressEnterToContinue();
                break;

            case DEBUG_CLOSE_MENU:
            case INPUT_END_OF_STREAM:
                gm->debugOpen = false;
                return;

            default:
                break;
        }
    }
}
