#pragma once
#include <stdbool.h>

#define DECK_SIZE 52
#define MAX_ROOM_SIZE 4
#define MAX_MONSTER_WEAPON_STACK 26
#define STARTING_HEALTH 20
#define MINIMUM_HEALTH 0

// ========================================================
// Rules types
// ========================================================
typedef enum {
    WEAPON_NONE,
    WEAPON_FRESH,
    WEAPON_VALID_COMBO,
    WEAPON_INVALID_COMBO
} WeaponState;

typedef enum {
    COMBAT_CHOICE_USE_WEAPON = 1,
    COMBAT_CHOICE_BARE_HANDED = 2
} CombatChoice;

typedef enum {
    CONFIRM_YES = 1,
    CONFIRM_NO = 2
} ConfirmInput;

typedef enum {
    ENCOUNTER_RESOLVED,
    ENCOUNTER_CANCELLED,
    ENCOUNTER_BLOCKED_EMPTY_SLOT,
    ENCOUNTER_BLOCKED_ROOM_NOT_CLEARED
} EncounterResult;

typedef enum {
    ENCOUNTER_PROMPT_NONE,
    ENCOUNTER_PROMPT_COMBAT_CHOICE,
    ENCOUNTER_PROMPT_BARE_HANDED_CONFIRM,
    ENCOUNTER_PROMPT_WEAPON_SWAP,
    ENCOUNTER_PROMPT_POTION_WASTE
} EncounterPrompt;


typedef enum {
    FLEE_RESOLVED,
    FLEE_BLOCKED
} FleeResult;

typedef enum {
    EMPTY = 'E',
    MONSTER = 'M',
    POTION = 'P',
    WEAPON = 'W',
    FLEE = 'F'
} EncounterType;

// ========================================================
// Input codes
// ========================================================
typedef enum {
    INPUT_END_OF_STREAM = -2,
    INPUT_INVALID = -1,
    INPUT_DEBUG_COMMAND = 99
} InputSignal;

typedef enum {
    INPUT_SLOT_0 = 1,
    INPUT_SLOT_1 = 2,
    INPUT_SLOT_2 = 3,
    INPUT_SLOT_3 = 4,
    INPUT_FLEE = 5,
    INPUT_PAUSE = 9
} PlayerInput;

typedef enum {
    MENU_QUIT = 0,
    MENU_START_GAME = 1,
    MENU_OPTIONS = 2,
    MENU_DEBUG = 9
} MainMenuInput;

typedef enum {
    PAUSE_RESUME = 1,
    PAUSE_OPTIONS = 2,
    PAUSE_DEBUG = 3,
    PAUSE_QUIT = 9
} PauseMenuInput;

typedef enum {
    OPTIONS_TOGGLE_DEBUG = 1,
    OPTIONS_TOGGLE_AUTO_COMBAT = 2,
    OPTIONS_TOGGLE_AUTO_EQUIP = 3,
    OPTIONS_BACK = 9
} OptionsMenuInput;

typedef enum {
    DEBUG_PRINT_MAIN_DECK = 1,
    DEBUG_PRINT_DISCARD_PILE = 2,
    DEBUG_PRINT_TEMP_DECK = 3,
    DEBUG_PRINT_PLAYER_STATS = 4,
    DEBUG_PRINT_DUNGEON_ROOM = 5,
    DEBUG_CLOSE_MENU = 9
} DebugMenuInput;

// ========================================================
// State machines
// ========================================================
typedef enum {
    SYSTEM_EXIT = 0,
    SYSTEM_MAIN_MENU = 1,
    SYSTEM_IN_GAME = 2,
    SYSTEM_OPTIONS = 3
} SystemState;

typedef enum {
    PLAYING_ACTIVE,
    PLAYING_PAUSED,
    PLAYING_OPTIONS,
    PLAYING_GAMEOVER,
    PLAYING_EXIT
} InGameState;

// ========================================================
// Session data
// ========================================================
typedef struct {
    bool debugMenuEnabled;
    bool debugOpen;
    bool autoResolveCombat;
    bool autoConfirmWeaponSwap;
    SystemState gameState;
    unsigned int rngSeed;
} GameMaster;

typedef struct {
    int id;
    char type;
    int value;
} Card;

typedef struct CardLink {
    Card* data;
    struct CardLink* next;
} CardLink;

typedef struct {
    CardLink* topCard;
    CardLink* bottomCard;
    int count;
} Zone;

typedef struct {
    CardLink* equipped;
    CardLink* monsterStack[MAX_MONSTER_WEAPON_STACK];
    int killCount;
} Weapon;

typedef struct {
    int minHealth;
    int maxHealth;
    int health;
    Weapon weapon;
    bool canFlee;
    bool potionUsedThisTurn;
} Player;

typedef struct {
    Card globalCardPool[DECK_SIZE];
    CardLink nodePool[DECK_SIZE];
    Zone mainDeck;
    Zone discardPile;
    CardLink* roomSlots[MAX_ROOM_SIZE];
    Player playerOne;
    Card* lastResolvedCard;
} Game;
