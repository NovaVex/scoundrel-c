#include "Scene_manager.h"
#include "Data_Structure.h"
#include "Game_mechanics.h"
#include "UI_manager.h"
#include "Input.h"
#include <stdio.h>
#include <stdbool.h>

// ========================================================
// Local word helpers
// ========================================================
static const char* onOrOff(bool state) {
    if (state) return "ON";
    return "OFF";
}

// ========================================================
// CORE SCENES
// ========================================================
int openMainMenu() {
    clearScreen();

    printf("==============================\n");
    printf("          SCOUNDREL           \n");
    printf("==============================\n");
    printf("1. Start Game\n");
    printf("2. Options\n");
    printf("9. Debug Menu\n");
    printf("0. Quit\n");
    printf("==============================\n");
    printf("Select an option: ");

    return processUserInput();
}

int runActiveGameScene(struct game* session, bool debugViewEnabled) {
    clearScreen();

    renderGameView(session);
    if (debugViewEnabled) debugRender(session);

    printf("\n=== ACTIONS ===\n");

    for (int slotIndex = 0; slotIndex < MAX_ROOM_SIZE; slotIndex++) {
        if (isRoomSlotEmpty(session, slotIndex)) {
            printf("%d. [Empty Slot]\n", slotIndex + 1);
            continue;
        }
        printf("%d. Encounter Slot %d\n", slotIndex + 1, slotIndex + 1);
    }

    if (session->playerOne.canFlee) printf("5. Flee the Room\n");
    if (session->playerOne.canFlee == false) printf("5. [Cannot Flee]\n");

    if (roomIsReadyToLeave(session)) printf("8. Move to Next Room\n");

    printf("9. Pause Game\n");
    printf("=================\n");
    printf("Make your move: ");

    return processUserInput();
}

int runPauseScene() {
    clearScreen();

    printf("=== PAUSED ===\n");
    printf("1. Resume Game\n");
    printf("2. Options\n");
    printf("9. Quit to Main Menu\n");
    printf("=================\n");
    printf("Select an option: ");

    return processUserInput();
}

int runOptionsScene(bool debugViewEnabled) {
    clearScreen();

    printf("=== OPTIONS ===\n");
    printf("1. Toggle Debug View (Current: %s)\n", onOrOff(debugViewEnabled));
    printf("9. Back\n");
    printf("=================\n");
    printf("Select an option: ");

    return processUserInput();
}

bool askFightWithWeapon(int weaponValue, int monsterValue) {
    int choice = -1;

    while (choice != 1 && choice != 2) {
        printf("\nA monster of value %d blocks your path!\n", monsterValue);
        printf("1. Fight with your weapon (%d)\n", weaponValue);
        printf("2. Fight barehanded (save the weapon)\n");
        printf("Select an option: ");
        choice = processUserInput();
    }

    return (choice == 1);
}

void runGameOverScene(int finalScore) {
    clearScreen();
    printGameOverScreen(finalScore);
    pressEnterToContinue();
}

void runVictoryScene(int finalScore) {
    clearScreen();
    printVictoryScreen(finalScore);
    pressEnterToContinue();
}

// ========================================================
// DEBUG SCENES
// ========================================================
void debugGenerateTempTestDeck() {
    struct game temporarySession = {0};
    int totalCards = 0;

    generateCardPool(&temporarySession, &totalCards);
    buildDeck(&temporarySession, totalCards);

    printEntireDeckLoop(&temporarySession.mainDeck);
    pressEnterToContinue();
}

void openDebugMenu(struct gameMaster* engine, struct game* activeSession) {
    if (engine->debugMenuEnabled == false) return;

    engine->debugOpen = true;

    while (engine->debugOpen) {
        clearScreen();

        printf("=== DEBUG MENU ===\n");
        printf("1. Print Main Deck\n");
        printf("2. Print Discard Pile\n");
        printf("3. Generate & Print Temp Deck\n");
        printf("4. Print Player Stats\n");
        printf("5. Print Dungeon Room\n");
        printf("9. Close Menu\n");
        printf("==================\n");
        printf("Select an option: ");

        int playerChoice = processUserInput();

        switch (playerChoice) {
            case 1:
                printSessionDeck(activeSession);
                pressEnterToContinue();
                break;

            case 2:
                if (activeSession == NULL) {
                    printf("ERROR: No active session running.\n");
                    pressEnterToContinue();
                    break;
                }
                printEntireDeckLoop(&activeSession->discardPile);
                pressEnterToContinue();
                break;

            case 3:
                debugGenerateTempTestDeck();
                break;

            case 4:
                if (activeSession == NULL) {
                    printf("ERROR: No active session running.\n");
                    pressEnterToContinue();
                    break;
                }
                printCurrentPlayerStats(activeSession);
                pressEnterToContinue();
                break;

            case 5:
                printDungeonRoom(activeSession);
                pressEnterToContinue();
                break;

            case 9:
                engine->debugOpen = false;
                return;

            default:
                break;
        }
    }
}
