#pragma once


// ========================================================
// Input codes
// ========================================================
typedef enum InputSignal {
    INPUT_END_OF_STREAM = -2,
    INPUT_INVALID = -1,
    INPUT_DEBUG_COMMAND = 99
} InputSignal;

typedef enum PlayerInput {
    INPUT_SLOT_0 = 1,
    INPUT_SLOT_1 = 2,
    INPUT_SLOT_2 = 3,
    INPUT_SLOT_3 = 4,
    INPUT_FLEE = 5,
    INPUT_PAUSE = 9
} PlayerInput;

typedef enum MainMenuInput {
    MENU_QUIT = 0,
    MENU_START_GAME = 1,
    MENU_OPTIONS = 2,
    MENU_DEBUG = 9
} MainMenuInput;

typedef enum PauseMenuInput {
    PAUSE_RESUME = 1,
    PAUSE_OPTIONS = 2,
    PAUSE_DEBUG = 3,
    PAUSE_QUIT = 9
} PauseMenuInput;

typedef enum OptionsMenuInput {
    OPTIONS_TOGGLE_DEBUG = 1,
    OPTIONS_TOGGLE_AUTO_COMBAT = 2,
    OPTIONS_TOGGLE_AUTO_EQUIP = 3,
    OPTIONS_BACK = 9
} OptionsMenuInput;

typedef enum DebugMenuInput {
    DEBUG_PRINT_MAIN_DECK = 1,
    DEBUG_PRINT_DISCARD_PILE = 2,
    DEBUG_PRINT_TEMP_DECK = 3,
    DEBUG_PRINT_PLAYER_STATS = 4,
    DEBUG_PRINT_DUNGEON_ROOM = 5,
    DEBUG_CLOSE_MENU = 9
} DebugMenuInput;

typedef enum ConfirmInput {
    CONFIRM_YES = 1,
    CONFIRM_NO = 2
} ConfirmInput;
