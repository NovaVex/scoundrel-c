#include "Game_master.h"
#include <stdlib.h>
#include <time.h>
#include "Data_Structure.h"
#include "Scene_manager.h"
#include "Game_mechanics.h"
#include "Input.h"
#include "UI_manager.h"

// ========================================================
// System entry
// ========================================================
int gameMaster() {
    struct gameMaster engine = {0};

    wakeGameMaster(&engine);

    while (engine.gameState >= 1) {
        switch (engine.gameState) {
            
            case 1: { // Main Menu
                int playerChoice = openMainMenu();
                
                switch (playerChoice) {
                    case 1:  engine.gameState = 2; break;
                    case 2:  engine.gameState = 3; break;
                    case 9:  openDebugMenu(&engine, NULL); break;
                    case 99: engine.debugMenuEnabled = !engine.debugMenuEnabled; break;
                    case 0:  engine.gameState = 0; break;
                    default: break; // Ignore invalid inputs
                }
                break;
            }

            case 2: // Active Game
                gameLoop(&engine);
                break;

            case 3: { // Options Menu
                int playerChoice = runOptionsScene(engine.debugMenuEnabled);
                
                switch (playerChoice) {
                    case 1:  engine.debugMenuEnabled = !engine.debugMenuEnabled; break;
                    case 9:  engine.gameState = 1; break;
                    default: break; 
                }
                break;
            }

            default:
                engine.gameState = 1;
                break;
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

    seedRandomNumberGenerator(startingValues);
}

void gameSetUp(struct game* session) {
    int totalCards = 0;

    setPlayerDefault(&session->playerOne);

    generateCardPool(session, &totalCards);
    buildDeck(session, totalCards);

    session->lastFacedType = EMPTY;
    session->lastFacedValue = 0;

    dealRoomCards(session);
}

void setPlayerDefault(struct player* playerOne) {
    playerOne->minimumHealth = 0;
    playerOne->maximumHealth = 20;
    playerOne->currentHealth = 20;
    playerOne->weapon.equippedCard = NULL;
    playerOne->weapon.killCount = 0;
    playerOne->canFlee = true;
    playerOne->potionUsedThisRoom = false;
}

void seedRandomNumberGenerator(struct gameMaster* engine) {
    unsigned int startingSeed = (unsigned int)time(NULL);
    srand(startingSeed);
    engine->randomSeed = startingSeed;
}

// ========================================================
// Game loop managers
// ========================================================
void gameLoop(struct gameMaster* engine) {
    struct game session = {0};
    InGameState currentGameState = PLAYING_ACTIVE;

    gameSetUp(&session);

    while (currentGameState != PLAYING_EXIT) {
        switch (currentGameState) {
            case PLAYING_ACTIVE:
                currentGameState = activeGameManager(&session, engine);
                break;

            case PLAYING_PAUSED:
                currentGameState = activeGamePauseManager();
                break;

            case PLAYING_OPTIONS:
                currentGameState = activeGameOptionsManager(engine);
                break;

            case PLAYING_GAMEOVER:
                currentGameState = activeGameOverManager(&session);
                break;

            case PLAYING_VICTORY:
                currentGameState = activeGameVictoryManager(&session);
                break;

            case PLAYING_EXIT:
            default:
                currentGameState = PLAYING_EXIT;
                break;
        }
    }

    engine->gameState = 1;
}

// ========================================================
// State sub-managers
// ========================================================
InGameState activeGameManager(struct game* session, struct gameMaster* engine) {
    if (isPlayerDead(&session->playerOne)) return PLAYING_GAMEOVER;
    if (isDungeonCleared(session)) return PLAYING_VICTORY;

    int playerChoice = runActiveGameScene(session, engine->debugMenuEnabled);

    return routePlayerAction(session, playerChoice);
}

InGameState activeGamePauseManager() {
    int playerChoice = runPauseScene();

    switch (playerChoice) {
        case 1:
            return PLAYING_ACTIVE;
        case 2:
            return PLAYING_OPTIONS;
        case 9:
            return PLAYING_EXIT;
        default:
            return PLAYING_PAUSED;
    }
}

InGameState activeGameOptionsManager(struct gameMaster* engine) {
    int playerChoice = runOptionsScene(engine->debugMenuEnabled);

    if (playerChoice == 1) {
        engine->debugMenuEnabled = !engine->debugMenuEnabled;
        return PLAYING_OPTIONS;
    }

    if (playerChoice == 9) return PLAYING_PAUSED;

    return PLAYING_OPTIONS;
}

InGameState activeGameOverManager(struct game* session) {
    runGameOverScene(calculateFinalScore(session));
    return PLAYING_EXIT;
}

InGameState activeGameVictoryManager(struct game* session) {
    runVictoryScene(calculateFinalScore(session));
    return PLAYING_EXIT;
}

// ========================================================
// Player action handlers
// ========================================================
InGameState routePlayerAction(struct game* session, int playerChoice) {
    switch ((PlayerInput)playerChoice) {
        case INPUT_SLOT_0:
        case INPUT_SLOT_1:
        case INPUT_SLOT_2:
        case INPUT_SLOT_3:
            handleSlotChoice(session, playerChoice - 1);
            break;

        case INPUT_FLEE:
            fleeManager(session);
            break;

        case INPUT_NEXT_ROOM:
            handleNextRoomChoice(session);
            break;

        case INPUT_PAUSE:
            return PLAYING_PAUSED;

        default:
            break;
    }

    return PLAYING_ACTIVE;
}

void handleSlotChoice(struct game* session, int slotIndex) {
    if (isRoomSlotEmpty(session, slotIndex)) return;

    if (mustKeepLastCardForNextRoom(session)) {
        printLastCardWarning();
        pressEnterToContinue();
        return;
    }

    bool fightBarehanded = resolveFightBarehanded(session, slotIndex);

    EncounterResult result = encounterManager(session, slotIndex, fightBarehanded);

    printEncounterMessage(result);
    if (result == ENCOUNTER_POTION_FIZZLED) pressEnterToContinue();
}

void handleNextRoomChoice(struct game* session) {
    if (roomIsReadyToLeave(session) == false) return;

    dealRoomCards(session);
    setCanFleeTrue(&session->playerOne);
}

bool resolveFightBarehanded(struct game* session, int slotIndex) {
    struct player* playerOne = &session->playerOne;
    struct card* facedCard = session->roomSlots[slotIndex]->data;

    if (facedCard->type != MONSTER) return false;
    if (weaponIsUsableAgainst(playerOne, facedCard) == false) return false;

    bool useWeapon = askFightWithWeapon(
        playerOne->weapon.equippedCard->value,
        facedCard->value);

    return (useWeapon == false);
}
