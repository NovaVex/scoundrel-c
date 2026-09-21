// ========================================================
// main.c  --  START HERE
// --------------------------------------------------------
// Hands control to gameMaster and does nothing else.
// The rest of this comment is a map of the project.
//
// WHERE TO CHANGE X
//   A game rule, damage, scoring, the deck   Game_mechanics.c
//   The words or layout on screen            Scene_manager.c
//   What a keypress does, menus, settings    Game_master.c
//   Key numbers for each menu                Terminal_Input.h
//   Reading the keyboard                     Input.c
//
// THE LAYER RULE
//   Only Scene_manager.c prints, and only Game_mechanics.c
//   changes game state. See STYLE_GUIDE.md for the rest.
// ========================================================

#include "Game_master.h"

int main(void) {
    gameMaster();

    return 0;
}
