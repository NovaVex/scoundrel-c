// ========================================================
// main.c  --  START HERE
// --------------------------------------------------------
// This file hands control to the engine and does nothing
// else. The rest of this comment is a map of the project.
//
// --------------------------------------------------------
// WHICH FILE IS A FUNCTION IN?
//
//   You can tell from the name alone. No searching needed:
//
//     render...   print...        -> UI_manager.c
//     open...     run...          -> Scene_manager.c
//     prompt...   report...       -> Scene_manager.c
//     everything else             -> Game_mechanics.c
//
//   The handful that break the pattern:
//     clearScreen, initializeDisplay      -> UI_manager.c
//     processUserInput,
//     pressEnterToContinue                -> Input.c
//     gameLoop, optionsLoop,
//     anything ...Manager                 -> Game_master.c
//
// --------------------------------------------------------
// WHICH FILE DO I OPEN TO CHANGE X?
//
//   A game rule, damage, scoring, the deck   Game_mechanics.c
//   The words on screen                      UI_manager.c
//   The order screens appear in              Scene_manager.c
//   What a keypress does                     Game_master.c
//   A shared type or enum                    Data_Structure.h
//
// --------------------------------------------------------
// THE LAYER RULE, IN ONE SENTENCE
//
//   Printing only happens in UI_manager.c, and rules only
//   happen in Game_mechanics.c. Everything else just carries
//   messages between those two.
//
//   Game_mechanics.c does not include stdio at all. If it
//   needs to refuse something, it returns a code and lets
//   Scene_manager.c pick the wording. That is the whole
//   reason the rules can be lifted into a game engine later
//   without touching them.
//
// --------------------------------------------------------
// EACH .c FILE HAS ITS OWN MAP AT THE TOP
//   Open the file and read the header comment first.
//   Game_master.c has a full walkthrough of a single turn.
// ========================================================

#include "Game_master.h"

int main(void) {

    gameMaster();

    return 0;
}
