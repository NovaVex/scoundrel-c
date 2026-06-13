#include "Scene_manager.h"
#include "Data_Structure.h"
#include "Game_mechanics.h"
#include "UI_manager.h"
#include "Input.h"
#include "Game_master.h"
#include <stdio.h>
#include <stdbool.h>

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
    printf("==============================\n");
    printf("Select an option: ");
    
    int userInput = processUserInput();
    return userInput;
}

int runActiveGameScene(struct game* session) {
    clearScreen();
    debugRender(session);
    
    int cardsLeft = countCardsInRoom(session);
    
    printf("\n=== ACTIONS ===\n");
    
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        if (!isRoomSlotEmpty(session, i)) {
            printf("%d. Encounter Slot %d\n", i + 1, i + 1);
        } else {
            printf("%d. [Empty Slot]\n", i + 1);
        }
    }
    
    if (session->playerOne.canFlee) {
        printf("5. Flee the Room\n");
    } else {
        printf("5. [Cannot Flee]\n");
    }
    
    if (cardsLeft <= 1) {
        printf("8. Move to Next Room\n");
    }
    
    printf("9. Pause Game\n");
    printf("=================\n");
    printf("Make your move: ");
    
    int userInput = processUserInput();
    return userInput;
}

// ========================================================
// DEBUG SCENES
// ========================================================
void debugGenerateTempTestDeck() {
    struct game tempSession = {0};
    int totalCards = 0;
    
    generateCardPool(&tempSession, &totalCards);
    buildDeck(&tempSession, totalCards);
    
    printEntireDeckLoop(&tempSession.mainDeck);
    pressEnterToContinue();
}

void openDebugMenu(struct gameMaster* engine, struct game* activeSession) {
    if (!engine->debugMenuEnabled) return; 
    
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
                if (isGameSessionActive(activeSession)) {
                    printSessionDeck(activeSession);
                } else {
                    printf("ERROR: No active session running.\n");
                }
                pressEnterToContinue();
                break;
                
            case 2: 
                if (isGameSessionActive(activeSession)) {
                    printEntireDeckLoop(&activeSession->discardPile);
                } else {
                    printf("ERROR: No active session running.\n");
                }
                pressEnterToContinue();
                break;
                
            case 3: 
                debugGenerateTempTestDeck();
                break;
                
            case 4: 
                if (isGameSessionActive(activeSession)) {
                    printCurrentPlayerStats(activeSession);
                } else {
                    printf("ERROR: No active session running.\n");
                }
                pressEnterToContinue();
                break;
                
            case 5: 
                if (isGameSessionActive(activeSession)) {
                    printDungeonRoom(activeSession);
                } else {
                    printf("ERROR: No active session running.\n");
                }
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