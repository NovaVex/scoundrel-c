// ========================================================
// Scene_manager.c
// --------------------------------------------------------
// Every screen: draw it, then ask. The only file that prints.
// Throwaway at the engine port, together with Input.c.
//
// Each screen is a picture of its output, then its render and
// ask functions. If you change a screen, change its picture.
//
// Basic utilities, Screen furniture, Messages, Menu screens,
// Play screens, Action prompts, Turn flow, then Debug.
// ========================================================

#include "Scene_manager.h"
#include "Terminal_Input.h"
#include "Game_mechanics.h"
#include "Input.h"
#include <stdio.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

// ========================================================
// Basic utilities
// ========================================================
void initializeDisplay(void) {
#ifdef _WIN32
    HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;

    if (outputHandle == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleMode(outputHandle, &consoleMode)) return;   // If the console mode can't be read, stop here

    SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);   // | adds the flag: the current mode, plus terminal codes
#endif
}

void clearScreen(void) {
    printf("\033[H\033[J");
    fflush(stdout);
}

// ========================================================
// Screen furniture
// --------------------------------------------------------
// The shared pieces every screen is built from, so no screen
// hand-counts a border or hardcodes an option number.
// const char* is text the function promises not to change.
// ========================================================
// prints:  ==============================
void renderRule(void) {
    printf("==============================\n");
}

// prints:  ------------------------------
void renderSeparator(void) {
    printf("------------------------------\n");
}

// renderTitle("PAUSED") prints a blank line, then:
//          === PAUSED ===
void renderTitle(const char* title) {
    printf("\n=== %s ===\n", title);
}

// renderMenuOption(1, "Start Game") prints:
//          1. Start Game
void renderMenuOption(int optionKey, const char* label) {
    printf("%d. %s\n", optionKey, label);
}

// renderToggleOption(2, "Auto-Resolve Combat", true) prints:
//          2. Auto-Resolve Combat (Current: ON)
void renderToggleOption(int optionKey, const char* label, bool isEnabled) {
    printf("%d. %s (Current: %s)\n", optionKey, label, isEnabled ? "ON" : "OFF");   // ON if enabled, otherwise OFF
}

// renderPrompt("Select an option") prints with no newline, so
// the player types on the same line:  Select an option: _
void renderPrompt(const char* label) {
    printf("%s: ", label);
}

// The Debug Menu entry, hidden until SETTING_DEBUG_MODE is on.
// Typing "debug" at the main menu turns it on (mainMenuManager).
void renderDebugLine(GameMaster* gm, int optionKey) {
    if (!isSettingOn(gm, SETTING_DEBUG_MODE)) return;   // If debug mode is off, draw nothing

    renderMenuOption(optionKey, "Debug Menu");
}

// renderMonsterTitle(11) prints a blank line, then:
//          === A 11 of M blocks your path ===
void renderMonsterTitle(int monsterValue) {
    printf("\n=== A %d of %c blocks your path ===\n", monsterValue, MONSTER);
}

// ========================================================
// Messages
// ========================================================
void renderNoActiveSession(void) {
    printf("ERROR: No active session running.\n");
}

void renderEmptySlotChosen(void) {
    printf("That slot is empty. Choose a slot with a card in it.\n");
}

void renderRoomNotCleared(void) {
    printf("This room is finished. You must move deeper into the dungeon.\n");
}

void renderFleeBlocked(void) {
    printf("You can't flee twice in a row. Encounter a card first.\n");
}

void renderInvalidSelection(void) {
    printf("That isn't one of the listed options.\n");
}

// ========================================================
// Menu screens
// ========================================================
// ------------------------------------------------
//  ==============================
//            SCOUNDREL
//              v0.1.2
//  ==============================
//  1. Start Game
//  2. Options
//  0. Quit
//  ==============================
//  Select an option: _
// ------------------------------------------------
//  "9. Debug Menu" only appears once debug mode is on.
// ------------------------------------------------
void renderMainMenu(GameMaster* gm) {
    renderRule();
    printf("          SCOUNDREL           \n");
    printf("            v%s\n", GAME_VERSION);
    renderRule();

    renderMenuOption(MENU_START_GAME, "Start Game");
    renderMenuOption(MENU_OPTIONS, "Options");
    renderDebugLine(gm, MENU_DEBUG);
    renderMenuOption(MENU_QUIT, "Quit");

    renderRule();
    renderPrompt("Select an option");
}

int openMainMenu(GameMaster* gm) {
    clearScreen();
    renderMainMenu(gm);

    int playerChoice = processUserInput();

    return playerChoice;
}

// ------------------------------------------------
//  === PAUSED ===
//  1. Resume Game
//  2. Options
//  9. Quit to Main Menu
//  ==============================
//  Select an option: _
// ------------------------------------------------
//  "3. Debug Menu" only appears once debug mode is on.
// ------------------------------------------------
void renderPauseMenu(GameMaster* gm) {
    renderTitle("PAUSED");

    renderMenuOption(PAUSE_RESUME, "Resume Game");
    renderMenuOption(PAUSE_OPTIONS, "Options");
    renderDebugLine(gm, PAUSE_DEBUG);
    renderMenuOption(PAUSE_QUIT, "Quit to Main Menu");

    renderRule();
    renderPrompt("Select an option");
}

int openPauseScene(GameMaster* gm) {
    clearScreen();
    renderPauseMenu(gm);

    int playerChoice = processUserInput();

    return playerChoice;
}

// ------------------------------------------------
//  === OPTIONS ===
//  2. Auto-Resolve Combat (Current: OFF)
//  3. Auto-Confirm Weapon Swap (Current: OFF)
//  9. Back
//  ==============================
//  Select an option: _
// ------------------------------------------------
//  With debug mode on, line 1 appears above the rest
//  (and switches it back off), and line 4 below them:
//  1. Debug Menu (Current: ON)
//  4. Rigged Test Deck (Current: OFF)
// ------------------------------------------------
void renderOptionsMenu(GameMaster* gm) {
    bool debugOn = isSettingOn(gm, SETTING_DEBUG_MODE);
    bool autoCombatOn = isSettingOn(gm, SETTING_AUTO_RESOLVE_COMBAT);
    bool autoEquipOn = isSettingOn(gm, SETTING_AUTO_CONFIRM_WEAPON_SWAP);
    bool riggedDeckOn = isSettingOn(gm, SETTING_RIGGED_DECK);

    renderTitle("OPTIONS");

    if (debugOn) {
        renderToggleOption(OPTIONS_TOGGLE_DEBUG, "Debug Menu", debugOn);
    }

    renderToggleOption(OPTIONS_TOGGLE_AUTO_COMBAT, "Auto-Resolve Combat", autoCombatOn);
    renderToggleOption(OPTIONS_TOGGLE_AUTO_EQUIP, "Auto-Confirm Weapon Swap", autoEquipOn);

    if (debugOn) {
        renderToggleOption(OPTIONS_TOGGLE_RIGGED_DECK, "Rigged Test Deck", riggedDeckOn);
    }

    renderMenuOption(OPTIONS_BACK, "Back");

    renderRule();
    renderPrompt("Select an option");
}

int openOptionsScene(GameMaster* gm) {
    clearScreen();
    renderOptionsMenu(gm);

    int playerChoice = processUserInput();

    return playerChoice;
}

// ========================================================
// Play screens
// ========================================================
// The main play screen, with renderActionMenu drawn under it:
// ------------------------------------------------
//  === SCOUNDREL ===
//  HP: 20/20
//  Weapon: [UNARMED]              <- renderWeaponLine
//  Cards in Deck: 40
//  Cards in Discard: 0
//  Can Flee Room? YES
//  Potion Used This Turn? NO
//  ------------------------------
//  Slot 1: [2 of M]               <- renderRoomSlots
//  Slot 2: [10 of M]
//  Slot 3: [7 of P]
//  Slot 4: [4 of M]
//  ==============================
// ------------------------------------------------
//  [value of type]: M = monster, P = potion, W = weapon.
// ------------------------------------------------
void renderGameState(Game* session) {
    Player* player = &session->playerOne;

    renderTitle("SCOUNDREL");
    printf("HP: %d/%d\n", player->health, PLAYER_MAX_HEALTH);

    renderWeaponLine(player); //decides what to print if the player is unarmed or has a weapon equipped

    printf("Cards in Deck: %d\n", session->mainDeck.count);
    printf("Cards in Discard: %d\n", session->discardPile.count);
    printf("Can Flee Room? %s\n", player->canFlee ? "YES" : "NO");                        // YES if the player can flee, otherwise NO
    printf("Potion Used This Turn? %s\n", player->potionUsedThisTurn ? "YES" : "NO");     // YES if a potion was drunk, otherwise NO

    renderSeparator();

    renderRoomSlots(session);

    renderRule();
}

// One line, three possible shapes:
//   Weapon: [UNARMED]                        no weapon
//   Weapon Value: 7 | Last Kill: [NONE]      equipped, no kills yet
//   Weapon Value: 7 | Last Kill: 9           equipped, has killed
void renderWeaponLine(Player* player) {
    if (player->weapon.equipped == NULL) {
        printf("Weapon: [UNARMED]\n");
        return;
    }

    int weaponValue = getEquippedWeaponValue(player);

    if (player->weapon.killCount == 0) {
        printf("Weapon Value: %d | Last Kill: [NONE]\n", weaponValue);
        return;
    }

    int lastKillValue = getLastKillValue(player);

    printf("Weapon Value: %d | Last Kill: %d\n", weaponValue, lastKillValue);
}

void renderRoomSlots(Game* session) {
    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        Card* slotCard = session->roomSlots[roomSlot];

        if (slotCard == NULL) {
            printf("Slot %d: [EMPTY]\n", roomSlot + SLOT_DISPLAY_OFFSET);
            continue;
        }

        printf("Slot %d: [%d of %c]\n", roomSlot + SLOT_DISPLAY_OFFSET, slotCard->value, slotCard->type);
    }
}

// Drawn directly under renderGameState:
// ------------------------------------------------
//  === ACTIONS ===
//  1. Encounter Slot 1
//  2. Encounter Slot 2
//  3. Encounter Slot 3
//  4. Encounter Slot 4
//  5. Flee the Room
//  9. Pause Game
//  ==============================
//  Make your move: _
// ------------------------------------------------
//  An emptied slot shows "1. [Empty Slot]" instead.
//  Line 5 shows "[Cannot Flee]" once fleeing is spent.
// ------------------------------------------------
void renderActionMenu(Game* session) {
    renderTitle("ACTIONS");

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        int optionKey = INPUT_SLOT_0 + roomSlot;   // slots are 0-3 inside, keys 1-4 on screen

        if (isRoomSlotEmpty(session, roomSlot)) {
            renderMenuOption(optionKey, "[Empty Slot]");
            continue;
        }

        printf("%d. Encounter Slot %d\n", optionKey, roomSlot + SLOT_DISPLAY_OFFSET);
    }

    // "Flee the Room" if the player can flee, otherwise "[Cannot Flee]"
    renderMenuOption(INPUT_FLEE, session->playerOne.canFlee ? "Flee the Room" : "[Cannot Flee]");
    renderMenuOption(INPUT_PAUSE, "Pause Game");

    renderRule();
    renderPrompt("Make your move");
}

int runActiveGameScene(Game* session) {
    clearScreen();
    renderGameState(session);
    renderActionMenu(session);

    int playerChoice = processUserInput();

    return playerChoice;
}

// Drawn under renderGameState, so the finished room stays in view:
// ------------------------------------------------
//  === TURN COMPLETE ===
//  You move deeper into the dungeon.
//  ==============================
//
//  Press [Enter] to continue..._
// ------------------------------------------------
void renderTurnComplete(void) {
    renderTitle("TURN COMPLETE");
    printf("You move deeper into the dungeon.\n");
    renderRule();
}

void runTurnCompleteScene(Game* session) {
    clearScreen();
    renderGameState(session);
    renderTurnComplete();
    pressEnterToContinue();
}

// ------------------------------------------------
//  === GAME OVER ===
//  You have fallen in the dungeon.
//  Final score: -182
//  ==============================
// ------------------------------------------------
//  On a win, line 2 is "You cleared the dungeon."
// ------------------------------------------------
void renderGameOver(bool playerDied, int finalScore) {
    renderTitle("GAME OVER");
    // "fallen" if the player died, otherwise "cleared"
    printf("%s\n", playerDied ? "You have fallen in the dungeon." : "You cleared the dungeon.");
    printf("Final score: %d\n", finalScore);
    renderRule();
}

void openGameOverScene(bool playerDied, int finalScore) {
    clearScreen();
    renderGameOver(playerDied, finalScore);
    pressEnterToContinue();
}

// ========================================================
// Action prompts
// ========================================================
// Drawn under renderGameState when the weapon can be used on
// this monster. Skipped when Auto-Resolve Combat is ON:
// ------------------------------------------------
//  Slot 1: [8 of W]                 <- renderGameState
//  Slot 2: [11 of M]
//  Slot 3: [3 of M]
//  Slot 4: [EMPTY]
//  ==============================
//
//  === A 11 of M blocks your path ===
//  1. Fight with your weapon (Value: 7 | Damage taken: 4)
//  2. Fight bare-handed (Damage taken: 11)
//  Choose: _
// ------------------------------------------------
//  Damage numbers come from previewDamageTaken, the
//  same maths combat uses, so they always match.
// ------------------------------------------------
void renderCombatChoicePrompt(Game* session, int chosenSlot) {
    int monsterValue = getSlotCardValue(session, chosenSlot);
    int weaponValue = getEquippedWeaponValue(&session->playerOne);
    int weaponDamage = previewDamageTaken(session, chosenSlot, COMBAT_CHOICE_USE_WEAPON);
    int bareHandedDamage = previewDamageTaken(session, chosenSlot, COMBAT_CHOICE_BARE_HANDED);

    renderMonsterTitle(monsterValue);

    printf("%d. Fight with your weapon (Value: %d | Damage taken: %d)\n",
        COMBAT_CHOICE_USE_WEAPON,
        weaponValue,
        weaponDamage);

    printf("%d. Fight bare-handed (Damage taken: %d)\n",
        COMBAT_CHOICE_BARE_HANDED,
        bareHandedDamage);

    renderPrompt("Choose");
}

CombatChoice promptCombatChoice(Game* session, int chosenSlot) {
    while (true) {
        clearScreen();
        renderGameState(session);
        renderCombatChoicePrompt(session, chosenSlot);

        int playerChoice = processUserInput();

        if (playerChoice == COMBAT_CHOICE_USE_WEAPON) return COMBAT_CHOICE_USE_WEAPON;
        if (playerChoice == COMBAT_CHOICE_BARE_HANDED) return COMBAT_CHOICE_BARE_HANDED;
        if (playerChoice == INPUT_END_OF_STREAM) return COMBAT_CHOICE_BARE_HANDED;

        renderInvalidSelection();
        pressEnterToContinue();
    }
}

// Drawn under renderGameState when the weapon can't be used
// on this monster. Skipped when Auto-Resolve Combat is ON:
// ------------------------------------------------
//  Slot 1: [8 of W]                 <- renderGameState
//  Slot 2: [11 of M]
//  Slot 3: [3 of M]
//  Slot 4: [EMPTY]
//  ==============================
//
//  === A 11 of M blocks your path ===
//  You have no weapon. You will fight bare-handed.
//  Damage taken: 11
//  1. Yes, fight it
//  2. No, pick something else
//  Choose: _
// ------------------------------------------------
//  With a worn weapon equipped, line 2 becomes:
//  Your weapon is too worn for this one (last kill: 4).
//  You will fight bare-handed.
// ------------------------------------------------
void renderBareHandedConfirm(Game* session, int chosenSlot) {
    Player* player = &session->playerOne;

    int monsterValue = getSlotCardValue(session, chosenSlot);
    int damageTaken = previewDamageTaken(session, chosenSlot, COMBAT_CHOICE_BARE_HANDED);

    renderMonsterTitle(monsterValue);

    if (player->weapon.equipped == NULL) {
        printf("You have no weapon. You will fight bare-handed.\n");
    } else {
        int lastKillValue = getLastKillValue(player);

        printf("Your weapon is too worn for this one (last kill: %d).\n", lastKillValue);
        printf("You will fight bare-handed.\n");
    }

    printf("Damage taken: %d\n", damageTaken);

    renderMenuOption(CONFIRM_YES, "Yes, fight it");
    renderMenuOption(CONFIRM_NO, "No, pick something else");

    renderPrompt("Choose");
}

// Drawn under renderGameState when a weapon is already
// equipped. Skipped when Auto-Confirm Weapon Swap is ON:
// ------------------------------------------------
//  Weapon Value: 7 | Last Kill: 9   <- renderGameState
//  ...
//  Slot 1: [8 of W]
//  Slot 2: [11 of M]
//  ==============================
//
//  Equipping this weapon will discard your current
//  weapon and its kill stack (1 card(s)).
//  1. Yes, equip it
//  2. No, leave it
//  Choose: _
// ------------------------------------------------
void renderWeaponSwapConfirm(Player* player) {
    int cardsAtRisk = pendingWeaponDiscardCount(player);

    printf("\nEquipping this weapon will discard your current weapon and its kill stack (%d card(s)).\n", cardsAtRisk);

    renderMenuOption(CONFIRM_YES, "Yes, equip it");
    renderMenuOption(CONFIRM_NO, "No, leave it");

    renderPrompt("Choose");
}

// Drawn under renderGameState for a second potion in one
// turn. No setting skips this one:
// ------------------------------------------------
//  HP: 14/20                        <- renderGameState
//  ...
//  Potion Used This Turn? YES
//  ------------------------------
//  Slot 1: [5 of P]
//  ==============================
//
//  You already drank a potion this turn. This one
//  will be wasted with no healing effect.
//  1. Drink it anyway
//  2. Leave it
//  Choose: _
// ------------------------------------------------
void renderPotionWasteConfirm(void) {
    printf("\nYou already drank a potion this turn. This one will be wasted with no healing effect.\n");

    renderMenuOption(CONFIRM_YES, "Drink it anyway");
    renderMenuOption(CONFIRM_NO, "Leave it");

    renderPrompt("Choose");
}

// The ask for all three yes/no screens above. Returns true for YES.
bool promptConfirm(Game* session, EncounterPrompt prompt, int chosenSlot) {
    while (true) {
        clearScreen();
        renderGameState(session);

        switch (prompt) {
            case ENCOUNTER_PROMPT_BARE_HANDED_CONFIRM:
                renderBareHandedConfirm(session, chosenSlot);
                break;

            case ENCOUNTER_PROMPT_WEAPON_SWAP:
                renderWeaponSwapConfirm(&session->playerOne);
                break;

            case ENCOUNTER_PROMPT_POTION_WASTE:
                renderPotionWasteConfirm();
                break;

            default:
                return true;
        }

        int playerChoice = processUserInput();

        if (playerChoice == CONFIRM_YES) return true;
        if (playerChoice == CONFIRM_NO) return false;
        if (playerChoice == INPUT_END_OF_STREAM) return false;

        renderInvalidSelection();
        pressEnterToContinue();
    }
}

// ========================================================
// Turn flow
// --------------------------------------------------------
// No screen of their own: pick a prompt from above, pass the
// answer to Game_mechanics, and report what comes back.
// ========================================================
EncounterResult runEncounterScene(Game* session, GameMaster* gm, int chosenSlot) {
    CombatChoice combatChoice = COMBAT_CHOICE_BARE_HANDED;
    EncounterPrompt prompt = requiredEncounterPrompt(session, chosenSlot);

    switch (prompt) {
        case ENCOUNTER_PROMPT_COMBAT_CHOICE:
            if (isSettingOn(gm, SETTING_AUTO_RESOLVE_COMBAT)) {
                combatChoice = COMBAT_CHOICE_USE_WEAPON;
                break;
            }
            combatChoice = promptCombatChoice(session, chosenSlot);
            break;

        case ENCOUNTER_PROMPT_BARE_HANDED_CONFIRM:
            if (isSettingOn(gm, SETTING_AUTO_RESOLVE_COMBAT)) break;
            if (!promptConfirm(session, prompt, chosenSlot)) return ENCOUNTER_CANCELLED;   // If the player said no
            break;

        case ENCOUNTER_PROMPT_WEAPON_SWAP:
            if (isSettingOn(gm, SETTING_AUTO_CONFIRM_WEAPON_SWAP)) break;
            if (!promptConfirm(session, prompt, chosenSlot)) return ENCOUNTER_CANCELLED;   // If the player said no
            break;

        case ENCOUNTER_PROMPT_POTION_WASTE:
            if (!promptConfirm(session, prompt, chosenSlot)) return ENCOUNTER_CANCELLED;   // If the player said no
            break;

        case ENCOUNTER_PROMPT_NONE:
        default:
            break;
    }

    EncounterResult result = encounterManager(session, chosenSlot, combatChoice);

    reportEncounterResult(result);

    return result;
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

// ========================================================
// Debug screens
// ========================================================
// ------------------------------------------------
//  === DEBUG MENU ===
//  1. Print Main Deck
//  2. Print Discard Pile
//  3. Generate & Print Temp Deck
//  4. Print Player Stats
//  5. Print Dungeon Room
//  9. Close Menu
//  ==============================
//  Select an option: _
// ------------------------------------------------
void renderDebugMenu(void) {
    renderTitle("DEBUG MENU");

    renderMenuOption(DEBUG_PRINT_MAIN_DECK, "Print Main Deck");
    renderMenuOption(DEBUG_PRINT_DISCARD_PILE, "Print Discard Pile");
    renderMenuOption(DEBUG_PRINT_TEMP_DECK, "Generate & Print Temp Deck");
    renderMenuOption(DEBUG_PRINT_PLAYER_STATS, "Print Player Stats");
    renderMenuOption(DEBUG_PRINT_DUNGEON_ROOM, "Print Dungeon Room");
    renderMenuOption(DEBUG_CLOSE_MENU, "Close Menu");

    renderRule();
    renderPrompt("Select an option");
}

void openDebugMenu(GameMaster* gm, Game* session) {
    if (!isSettingOn(gm, SETTING_DEBUG_MODE)) return;   // If debug mode is off, stop here

    gm->isDebugMenuOpen = true;

    while (gm->isDebugMenuOpen) {
        clearScreen();
        renderDebugMenu();

        int playerChoice = processUserInput();

        switch (playerChoice) {
            case DEBUG_PRINT_MAIN_DECK:
                if (!isGameSessionActive(session)) {   // If there is no game running
                    renderNoActiveSession();
                } else {
                    printSessionDeck(session);
                }
                pressEnterToContinue();
                break;

            case DEBUG_PRINT_DISCARD_PILE:
                if (!isGameSessionActive(session)) {   // If there is no game running
                    renderNoActiveSession();
                } else {
                    printDiscardPile(session);
                }
                pressEnterToContinue();
                break;

            case DEBUG_PRINT_TEMP_DECK:
                debugGenerateTempTestDeck();
                break;

            case DEBUG_PRINT_PLAYER_STATS:
                if (!isGameSessionActive(session)) {   // If there is no game running
                    renderNoActiveSession();
                } else {
                    printCurrentPlayerStats(session);
                }
                pressEnterToContinue();
                break;

            case DEBUG_PRINT_DUNGEON_ROOM:
                if (!isGameSessionActive(session)) {   // If there is no game running
                    renderNoActiveSession();
                } else {
                    printDungeonRoom(session);
                }
                pressEnterToContinue();
                break;

            case DEBUG_CLOSE_MENU:
            case INPUT_END_OF_STREAM:
                gm->isDebugMenuOpen = false;
                return;

            default:
                break;
        }
    }
}

void debugGenerateTempTestDeck(void) {
    Game tempSession = {0};

    int totalCards = generateGlobalCardPool(tempSession.globalCardPool);

    buildDeck(&tempSession, totalCards);
    cardShuffle(tempSession.mainDeck.cards, totalCards);

    printEntireDeckLoop(&tempSession.mainDeck);
    pressEnterToContinue();
}

// ========================================================
// Debug printing (targeted)
// ========================================================
void printCurrentPlayerStats(Game* session) {
    Player* player = &session->playerOne;
    int weaponValue = getEquippedWeaponValue(player);

    printf("Max HP: %d\n", PLAYER_MAX_HEALTH);
    printf("Current HP: %d\n", player->health);
    printf("Weapon Value: %d\n", weaponValue);
    printf("Kill Count: %d\n", player->weapon.killCount);
}

void printEntireDeckLoop(Zone* pile) {
    if (pile->count == 0) {
        printf("--- PILE IS EMPTY ---\n");
        renderSeparator();
        return;
    }

    for (int position = 0; position < pile->count; position++) {
        Card* cardData = cardAtPosition(pile, position);

        printf("Position: %d | Card ID: %d | Type: %c | Value: %d\n",
            position + 1,
            cardData->id,
            cardData->type,
            cardData->value);
    }

    printf("--- END OF PILE ---\n");
}

// ========================================================
// Debug tools (session level)
// ========================================================
void printSessionDeck(Game* session) {
    printEntireDeckLoop(&session->mainDeck);
}

void printDiscardPile(Game* session) {
    printEntireDeckLoop(&session->discardPile);
}

void printDungeonRoom(Game* session) {
    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        Card* slotCard = session->roomSlots[roomSlot];

        if (slotCard == NULL) {
            printf("Dungeon Slot %d | [EMPTY]\n", roomSlot + SLOT_DISPLAY_OFFSET);
            continue;
        }

        printf("Dungeon Slot %d | Card ID: %d | Type: %c | Value: %d\n",
            roomSlot + SLOT_DISPLAY_OFFSET,
            slotCard->id,
            slotCard->type,
            slotCard->value);
    }

    printf("--- END OF ROOM ---\n");
}
