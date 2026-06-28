#pragma once
#include <stdbool.h>

#define DECK_SIZE 52
#define MAX_ROOM_SIZE 4
#define MAX_MONSTER_WEAPON_STACK 13

typedef enum {
    WEAPON_NONE,
    WEAPON_FRESH,
    WEAPON_VALID_COMBO,
    WEAPON_INVALID_COMBO
} WeaponState;

typedef enum {
    EMPTY   = 'E',
    MONSTER = 'M',
    POTION  = 'P',
    WEAPON  = 'W'
} EncounterType;

typedef enum {
    ENCOUNTER_NOTHING,
    ENCOUNTER_MONSTER_FOUGHT,
    ENCOUNTER_POTION_HEALED,
    ENCOUNTER_POTION_FIZZLED,
    ENCOUNTER_WEAPON_EQUIPPED
} EncounterResult;

typedef enum {
    INPUT_SLOT_0    = 1,
    INPUT_SLOT_1    = 2,
    INPUT_SLOT_2    = 3,
    INPUT_SLOT_3    = 4,
    INPUT_FLEE      = 5,
    INPUT_NEXT_ROOM = 8,
    INPUT_PAUSE     = 9
} PlayerInput;

typedef enum {
    PLAYING_ACTIVE,
    PLAYING_PAUSED,
    PLAYING_OPTIONS,
    PLAYING_GAMEOVER,
    PLAYING_VICTORY,
    PLAYING_EXIT
} InGameState;

struct gameMaster {
    bool debugMenuEnabled;
    bool debugOpen;
    int gameState;
    unsigned int randomSeed;
};

struct card {
    int identifier;
    char type;
    int value;
};

typedef struct CardLink {
    struct card* data;
    struct CardLink* next;
} CardLink;

typedef struct {
    CardLink* topCard;
    CardLink* bottomCard;
    int count;
} Zone;

struct weapon {
    struct card* equippedCard;
    struct card* monsterStack[MAX_MONSTER_WEAPON_STACK];
    int killCount;
};

struct player {
    int minimumHealth;
    int maximumHealth;
    int currentHealth;
    struct weapon weapon;
    bool canFlee;
    bool potionUsedThisRoom;
};

struct game {
    struct card globalCardPool[DECK_SIZE];
    CardLink nodePool[DECK_SIZE];
    Zone mainDeck;
    Zone discardPile;
    CardLink* roomSlots[MAX_ROOM_SIZE];
    struct player playerOne;

    char lastFacedType;
    int  lastFacedValue;
};
