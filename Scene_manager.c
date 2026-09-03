// ========================================================
// Scene_manager.c
// --------------------------------------------------------
// JOB
//   A "scene" is one screen the player looks at. This file
//   owns every screen end to end: the picture of what it
//   looks like, the code that draws it, and the code that
//   reads the answer. Each ask function does the same three
//   steps in the same order:
//
//       1. clearScreen()          wipe the terminal
//       2. render...()            draw it
//       3. processUserInput()     read them   (Input.c)
//
//   That is the entire pattern. Once you see it, every ask
//   function in this file reads the same way.
//
//   Every printf in the game lives here. No other file is
//   allowed to print. That is the whole rule.
//
// TEMPORARY
//   Throwaway layer, together with Input.c. The engine port
//   replaces both; Game_mechanics and its header move over
//   untouched. Do not let anything important live here.
//
// WHO CALLS THIS FILE
//   Game_master.c, and nothing else.
//
// WHAT THIS FILE CALLS
//   Game_mechanics.h   to ask what is legal, to ask for
//                      numbers to display, and to apply the
//                      player's choice once it is made
//   Input.h            to read the keyboard
//
// THE ONE RULE
//   This file decides HOW to ask a question. It never decides
//   WHAT the rules are. Every rule question goes to
//   Game_mechanics.c and comes back as a plain answer.
//
// READING THIS FILE
//   Each screen is one block: a picture of its output copied
//   from the real program, then the function that draws it,
//   then the function that asks. You should never have to run
//   the code in your head to know what a screen looks like.
//   If you change a screen, change its picture too.
//
// MAP OF THIS FILE, IN ORDER
//   BASIC UTILITIES   console setup and clearing
//   SCREEN FURNITURE  the shared pieces every screen is built from
//   MESSAGES          one-line notices, no menu attached
//   MENU SCREENS      main menu, pause, options
//   PLAY SCREENS      the room, end of turn, game over
//   ACTION PROMPTS    the four screens that ask a question
//   TURN FLOW         encounters and fleeing, which own no screen
//   DEBUG SCREENS     developer tools, not part of normal play
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
    if (!GetConsoleMode(outputHandle, &consoleMode)) return;

    SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void clearScreen(void) {
    printf("\033[H\033[J");
    fflush(stdout);
}

// ========================================================
// Screen furniture
// --------------------------------------------------------
// Every divider, option line and prompt in the game is drawn
// through one of these. The border strings exist in exactly
// one place each, so no screen hand-counts a width and no
// screen hardcodes an option number.
//
// NOTE ON "const char* label"
//   This is the first place in the project that takes text as
//   a parameter, so here is what it means:
//
//     char*         a pointer to text
//     const char*   a pointer to text this function promises
//                   not to change
//
//   When you call renderTitle("PAUSED"), the "PAUSED" lives in
//   the program's read-only memory and only its address gets
//   passed in. The const is a promise to the compiler, which
//   will refuse to build if the function tries to edit it.
//   Nothing is copied, and nothing needs freeing.
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
    printf("%d. %s (Current: %s)\n", optionKey, label, isEnabled ? "ON" : "OFF");
}

// renderPrompt("Select an option") prints, with no newline,
// so the player's typing appears on the same line:
//          Select an option: _
void renderPrompt(const char* label) {
    printf("%s: ", label);
}

// The easter egg, in one place. Every menu that can open the
// debug tools draws its entry through this, so the option is
// invisible until gm->debugMenuEnabled is set, and all three
// menus reveal and hide their entry together.
//
// The word is armed in routeMainMenuChoice (Game_master.c),
// which is what Input.c's INPUT_DEBUG_COMMAND lands on.
void renderDebugLine(GameMaster* gm, int optionKey) {
    if (!gm->debugMenuEnabled) return;

    renderMenuOption(optionKey, "Debug Menu");
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
// MENU SCREENS
// ========================================================
// ------------------------------------------------
//  ==============================
//            SCOUNDREL
//              v0.1.1
//  ==============================
//  1. Start Game
//  2. Options
//  0. Quit
//  ==============================
//  Select an option: _
// ------------------------------------------------
//  "9. Debug Menu" is missing on purpose. It only
//  appears once the player types the word "debug"
//  at this menu. See renderDebugLine.
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

    return processUserInput();
}

// ------------------------------------------------
//  === PAUSED ===
//  1. Resume Game
//  2. Options
//  9. Quit to Main Menu
//  ==============================
//  Select an option: _
// ------------------------------------------------
//  "3. Debug Menu" appears here only while the
//  easter egg is armed. See renderDebugLine.
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

    return processUserInput();
}

// ------------------------------------------------
//  === OPTIONS ===
//  2. Auto-Resolve Combat (Current: OFF)
//  3. Auto-Confirm Weapon Swap (Current: OFF)
//  9. Back
//  ==============================
//  Select an option: _
//
//  The ON/OFF values are read live from gm, so this
//  screen redraws with whatever is set.
//
//  Once the easter egg is armed, line 1 appears
//  above the rest and is the way to switch it back
//  off again:
//  1. Debug Menu (Current: ON)
// ------------------------------------------------
void renderOptionsMenu(GameMaster* gm) {
    renderTitle("OPTIONS");

    if (gm->debugMenuEnabled) {
        renderToggleOption(OPTIONS_TOGGLE_DEBUG, "Debug Menu", gm->debugMenuEnabled);
    }

    renderToggleOption(OPTIONS_TOGGLE_AUTO_COMBAT, "Auto-Resolve Combat", gm->autoResolveCombat);
    renderToggleOption(OPTIONS_TOGGLE_AUTO_EQUIP, "Auto-Confirm Weapon Swap", gm->autoConfirmWeaponSwap);
    renderMenuOption(OPTIONS_BACK, "Back");

    renderRule();
    renderPrompt("Select an option");
}

int openOptionsScene(GameMaster* gm) {
    clearScreen();
    renderOptionsMenu(gm);

    return processUserInput();
}

// ========================================================
// PLAY SCREENS
// ========================================================
// The main play screen. renderActionMenu is drawn straight
// after this one, so the player sees both together:
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
//  M = monster, P = potion, W = weapon.
//  The number is the card's value.
// ------------------------------------------------
void renderGameState(Game* session) {
    Player* player = &session->playerOne;

    renderTitle("SCOUNDREL");
    printf("HP: %d/%d\n", player->health, player->maxHealth);

    renderWeaponLine(player); //decides what to print if the player is unarmed or has a weapon equipped

    printf("Cards in Deck: %d\n", session->mainDeck.count);
    printf("Cards in Discard: %d\n", session->discardPile.count);
    printf("Can Flee Room? %s\n", player->canFlee ? "YES" : "NO");
    printf("Potion Used This Turn? %s\n", player->potionUsedThisTurn ? "YES" : "NO");

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

    if (player->weapon.killCount == 0) {
        printf("Weapon Value: %d | Last Kill: [NONE]\n", getEquippedWeaponValue(player));
        return;
    }

    printf("Weapon Value: %d | Last Kill: %d\n", getEquippedWeaponValue(player), getLastKillValue(player));
}

void renderRoomSlots(Game* session) {
    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        CardLink* slotLink = session->roomSlots[roomSlot];

        if (slotLink == NULL) {
            printf("Slot %d: [EMPTY]\n", roomSlot + 1);
            continue;
        }

        printf("Slot %d: [%d of %c]\n", roomSlot + 1, slotLink->data->value, slotLink->data->type);
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
//  This menu only ever appears when a move is legal;
//  a finished room is intercepted by Game_master.c
//  before this is ever drawn.
// ------------------------------------------------
void renderActionMenu(Game* session) {
    renderTitle("ACTIONS");

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        // Slots are counted 0-3 internally but shown as keys 1-4.
        // INPUT_SLOT_0 is 1, and the slot inputs are consecutive
        // in the enum, so adding the loop counter gives the key.
        int optionKey = INPUT_SLOT_0 + roomSlot;

        if (isRoomSlotEmpty(session, roomSlot)) {
            renderMenuOption(optionKey, "[Empty Slot]");
            continue;
        }

        printf("%d. Encounter Slot %d\n", optionKey, roomSlot + 1);
    }

    renderMenuOption(INPUT_FLEE, session->playerOne.canFlee ? "Flee the Room" : "[Cannot Flee]");
    renderMenuOption(INPUT_PAUSE, "Pause Game");

    renderRule();
    renderPrompt("Make your move");
}

int runActiveGameScene(Game* session) {
    clearScreen();
    renderGameState(session);
    renderActionMenu(session);

    return processUserInput();
}

// Drawn UNDER renderGameState, so the player sees the room
// they just finished before pressing on:
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
//
//  Line 2 is either "You have fallen in the dungeon."
//  or "You cleared the dungeon."
// ------------------------------------------------
void renderGameOver(bool playerDied, int finalScore) {
    renderTitle("GAME OVER");
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
// ACTION PROMPTS
// ========================================================
// Drawn in the action menu's place, under renderGameState, so
// the player keeps their stats and the room in front of them
// while they decide. Shown when the weapon is actually usable
// on this monster, and only when Auto-Resolve Combat is OFF:
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
//  Both damage numbers come from previewDamageTaken in
//  Game_mechanics.c, the same function combat itself
//  uses, so the preview can never disagree with reality.
// ------------------------------------------------
void renderCombatChoicePrompt(Game* session, int chosenSlot) {
    printf("\n=== A %d of %c blocks your path ===\n", getSlotCardValue(session, chosenSlot), MONSTER);

    printf("%d. Fight with your weapon (Value: %d | Damage taken: %d)\n",
        COMBAT_CHOICE_USE_WEAPON,
        getEquippedWeaponValue(&session->playerOne),
        previewDamageTaken(session, chosenSlot, COMBAT_CHOICE_USE_WEAPON));

    printf("%d. Fight bare-handed (Damage taken: %d)\n",
        COMBAT_CHOICE_BARE_HANDED,
        previewDamageTaken(session, chosenSlot, COMBAT_CHOICE_BARE_HANDED));

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

// Drawn in the action menu's place, under renderGameState, so
// the player keeps their stats and the room in front of them
// while they decide. Shown when the weapon cannot be used on
// this monster and the hit will land in full, and only when
// Auto-Resolve Combat is OFF:
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
//  The "no weapon" line has two shapes, the same split
//  renderWeaponLine makes. With a worn weapon equipped:
//  Your weapon is too worn for this one (last kill: 4).
//  You will fight bare-handed.
// ------------------------------------------------
//  The damage number comes from previewDamageTaken in
//  Game_mechanics.c, the same function combat itself uses.
// ------------------------------------------------
void renderBareHandedConfirm(Game* session, int chosenSlot) {
    Player* player = &session->playerOne;

    printf("\n=== A %d of %c blocks your path ===\n", getSlotCardValue(session, chosenSlot), MONSTER);

    if (player->weapon.equipped == NULL) {
        printf("You have no weapon. You will fight bare-handed.\n");
    } else {
        printf("Your weapon is too worn for this one (last kill: %d).\n", getLastKillValue(player));
        printf("You will fight bare-handed.\n");
    }

    printf("Damage taken: %d\n", previewDamageTaken(session, chosenSlot, COMBAT_CHOICE_BARE_HANDED));

    renderMenuOption(CONFIRM_YES, "Yes, fight it");
    renderMenuOption(CONFIRM_NO, "No, pick something else");

    renderPrompt("Choose");
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

// Drawn in the action menu's place, under renderGameState, so
// the weapon being given up and its kill count stay on screen
// while the player decides. Only shown when a weapon is already
// equipped, and only when Auto-Confirm Weapon Swap is OFF:
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
//  The count comes from pendingWeaponDiscardCount, so it
//  always matches what discardEquippedWeapon will throw away.
// ------------------------------------------------
void renderWeaponSwapConfirm(int cardsAtRisk) {
    printf("\nEquipping this weapon will discard your current weapon and its kill stack (%d card(s)).\n", cardsAtRisk);

    renderMenuOption(CONFIRM_YES, "Yes, equip it");
    renderMenuOption(CONFIRM_NO, "No, leave it");

    renderPrompt("Choose");
}

bool promptWeaponSwapConfirm(Game* session) {
    while (true) {
        clearScreen();
        renderGameState(session);
        renderWeaponSwapConfirm(pendingWeaponDiscardCount(&session->playerOne));

        int playerChoice = processUserInput();

        if (playerChoice == CONFIRM_YES) return true;
        if (playerChoice == CONFIRM_NO) return false;
        if (playerChoice == INPUT_END_OF_STREAM) return false;

        renderInvalidSelection();
        pressEnterToContinue();
    }
}

// Drawn in the action menu's place, under renderGameState, so
// the "Potion Used This Turn? YES" line that causes this warning
// is visible right above it. Only shown for a second potion in
// the same turn. No toggle skips this one; wasting a potion
// always asks:
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

bool promptPotionWasteConfirm(Game* session) {
    while (true) {
        clearScreen();
        renderGameState(session);
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
// TURN FLOW
// --------------------------------------------------------
// These own no screen of their own. They pick which prompt
// above to run, hand the answer to Game_mechanics.c, and
// report whatever comes back.
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

            if (!promptWeaponSwapConfirm(session)) {
                return ENCOUNTER_CANCELLED;
            }
            break;

        case ENCOUNTER_PROMPT_POTION_WASTE:
            if (!promptPotionWasteConfirm(session)) return ENCOUNTER_CANCELLED;
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
// DEBUG SCREENS
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

void debugGenerateTempTestDeck(void) {
    Game tempSession = {0};
    int totalCards = 0;

    generateCardPool(&tempSession, &totalCards);
    buildDeck(&tempSession, totalCards);

    printEntireDeckLoop(&tempSession.mainDeck);
    pressEnterToContinue();
}

// ========================================================
// Debug printing (targeted)
// ========================================================
void printCurrentPlayerStats(Game* session) {
    if (!isGameSessionActive(session)) {
        renderNoActiveSession();
        return;
    }

    Player* player = &session->playerOne;

    printf("Max HP: %d\n", player->maxHealth);
    printf("Current HP: %d\n", player->health);
    printf("Weapon Value: %d\n", getEquippedWeaponValue(player));
    printf("Kill Count: %d\n", player->weapon.killCount);
}

void printEntireDeckLoop(Zone* pile) {
    CardLink* currentLink = pile->topCard;
    int position = 1;

    if (currentLink == NULL) {
        printf("--- PILE IS EMPTY ---\n");
        renderSeparator();
        return;
    }

    while (currentLink != NULL) {
        Card* cardData = currentLink->data;

        printf("Position: %d | Card ID: %d | Type: %c | Value: %d\n",
            position,
            cardData->id,
            cardData->type,
            cardData->value);

        currentLink = currentLink->next;
        position++;
    }

    printf("--- END OF PILE ---\n");
}

// ========================================================
// Debug tools (session level)
// ========================================================
void printSessionDeck(Game* session) {
    if (!isGameSessionActive(session)) {
        renderNoActiveSession();
        return;
    }

    printEntireDeckLoop(&session->mainDeck);
}

void printDiscardPile(Game* session) {
    if (!isGameSessionActive(session)) {
        renderNoActiveSession();
        return;
    }

    printEntireDeckLoop(&session->discardPile);
}

void printDungeonRoom(Game* session) {
    if (!isGameSessionActive(session)) {
        renderNoActiveSession();
        return;
    }

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        CardLink* slotLink = session->roomSlots[roomSlot];

        if (slotLink == NULL) {
            printf("Dungeon Slot %d | [EMPTY]\n", roomSlot + 1);
            continue;
        }

        printf("Dungeon Slot %d | Card ID: %d | Type: %c | Value: %d\n",
            roomSlot + 1,
            slotLink->data->id,
            slotLink->data->type,
            slotLink->data->value);
    }

    printf("--- END OF ROOM ---\n");
}