#include "Game_master.h"
#include <stdlib.h>
#include <time.h>
#include "Data_Structure.h"
#include "Scene_manager.h"
#include "Game_mechanics.h"
#include "Input.h"
#include "UI_manager.h"
#include <stdio.h>

// ========================================================
// System entry
// ========================================================
int gameMaster() {
    struct gameMaster gm = {0};
    int playerChoice = 0;
    
    wakeGameMaster(&gm);

    while (gm.gameState >= 1) {
        switch (gm.gameState) {
            case 1:
                playerChoice = openMainMenu();

                switch (playerChoice) {
                    case 1:
                        gm.gameState = 2; 
                        break;
                    case 2:
                        gm.gameState = 3; 
                        break;
                    case 9:
                        openDebugMenu(&gm, NULL);
                        break;
                    case 99:
                        gm.debugMenuEnabled = !gm.debugMenuEnabled;
                        break;
                    default:
                        break;
                }
                break;

            case 2:
                gameLoop(&gm);
                break;

            case 3:
                break;
                
            case 9:
                openDebugMenu(&gm, NULL);
                continue;
        }
    }
    
    return 0;
}

// ========================================================
// Initialization
// ========================================================
void wakeGameMaster(struct gameMaster* startingValues) {
    startingValues->debugMenuEnabled = false;
    startingValues->debugOpen = false;
    startingValues->gameState = 1;
    
    randomNumberGenerator(startingValues);
}

void gameSetUp(struct game* session) {
    int totalCards = 0;
    
    setPlayerDefault(&(session->playerOne));    
    generateCardPool(session, &totalCards);
    buildDeck(session, totalCards);
    
    dealRoomCards(session);
}

void setPlayerDefault(struct player* playerOne) {
    playerOne->minHP = 0;
    playerOne->maxHP = 20;
    playerOne->hp = 20;
    playerOne->weapon.equipped = NULL;
    playerOne->canFlee = true;
}

void randomNumberGenerator(struct gameMaster* engine) {
    unsigned int startingSeed = (unsigned int)time(NULL);

    srand(startingSeed);

    int rngToSkip = rand() % 100;

    for (int rngIncrementer = 0; rngIncrementer < rngToSkip; rngIncrementer++) {
        rand();
    }

    engine->rngSeed = startingSeed;
}

// ========================================================
// Game loop managers
// ========================================================
void gameLoop(struct gameMaster* gm) {
    struct game session = {0};
    InGameState currentGameState = PLAYING_ACTIVE;
    
    gameSetUp(&session);        
    
    while (currentGameState != PLAYING_EXIT) {
        switch (currentGameState) {
            case PLAYING_ACTIVE:
                currentGameState = activeGameManger(&session, gm);
                break;
                
            case PLAYING_PAUSED:
                currentGameState = activeGamePauseManger(&session, gm);
                break;
                
            case PLAYING_OPTIONS:
                currentGameState = activeGameOptionsManger(&session, gm);
                break;
                
            case PLAYING_GAMEOVER:
                currentGameState = activeGameOverManger(&session, gm);
                break;
                
            case PLAYING_EXIT:
            default:
                currentGameState = PLAYING_EXIT;
                break;
        }
    }
    
    gm->gameState = 1;
}

// ========================================================
// State sub-managers
// ========================================================
InGameState activeGameManger(struct game* session, struct gameMaster* gm) {
    struct player* playerOne = &session->playerOne;

    if (isPlayerDead(playerOne)) {
        return PLAYING_GAMEOVER;
    }

    PlayerInput playerChoice = (PlayerInput)runActiveGameScene(session);

    switch (playerChoice) {
        case INPUT_SLOT_0:
        case INPUT_SLOT_1:
        case INPUT_SLOT_2:
        case INPUT_SLOT_3:
            encounterManager(session, playerChoice - 1);
            break;
            
        case INPUT_FLEE:
            fleeManager(session);
            break;
            
        case INPUT_NEXT_ROOM:
            if (countCardsInRoom(session) <= 1) {
                dealRoomCards(session);
                setCanFleeTrue(playerOne); 
            }
            break;
            
        case INPUT_PAUSE:
            return PLAYING_PAUSED;
            
        default:
            break;
    }

    return PLAYING_ACTIVE;
}

InGameState activeGamePauseManger(struct game* session, struct gameMaster* gm) {
    clearScreen();
    printf("=== PAUSED ===\n");
    printf("1. Resume Game\n");
    printf("9. Quit to Main Menu\n");
    printf("=================\n");
    printf("Select an option: ");
    
    int choice = processUserInput();
    if (choice == 1) return PLAYING_ACTIVE;
    if (choice == 9) return PLAYING_EXIT;
    
    return PLAYING_PAUSED;
}

InGameState activeGameOptionsManger(struct game* session, struct gameMaster* gm) {
    clearScreen();
    printf("=== OPTIONS ===\n");
    printf("1. Toggle Debug Menu (Current: %s)\n", gm->debugMenuEnabled ? "ON" : "OFF");
    printf("9. Back\n");
    printf("=================\n");
    printf("Select an option: ");
    
    int choice = processUserInput();
    if (choice == 1) {
        gm->debugMenuEnabled = !gm->debugMenuEnabled;
    } else if (choice == 9) {
        return PLAYING_ACTIVE;
    }
    
    return PLAYING_OPTIONS;
}

InGameState activeGameOverManger(struct game* session, struct gameMaster* gm) {
    clearScreen();
    printf("\n\n=== GAME OVER ===\n");
    printf("You have fallen in the dungeon.\n");
    pressEnterToContinue();
    return PLAYING_EXIT;
}

// ========================================================
// Helpers
// ========================================================
bool isGameSessionActive(struct game* session) {
    return (session != NULL);
}