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
    EMPTY = 'E',
    MONSTER = 'M',
    POTION = 'P',
    WEAPON = 'W',
    FLEE = 'F'
} EncounterType;

typedef enum {
    INPUT_SLOT_0 = 1,
    INPUT_SLOT_1 = 2,
    INPUT_SLOT_2 = 3,
    INPUT_SLOT_3 = 4,
    INPUT_FLEE = 5,
    INPUT_NEXT_ROOM = 8,
    INPUT_PAUSE = 9
} PlayerInput;

typedef enum {
    PLAYING_ACTIVE,
    PLAYING_PAUSED,
    PLAYING_OPTIONS,
    PLAYING_GAMEOVER,
    PLAYING_EXIT
} InGameState;

struct gameMaster {
    bool debugMenuEnabled;
    bool debugOpen;
    int gameState; 
    int rngSeed;
};

struct card {
    int id;
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
    struct card* equipped;
    struct card* monsterStack[MAX_MONSTER_WEAPON_STACK];
    int killCount;
};

struct player {
    int minHP;
    int maxHP;
    int hp;
    struct weapon weapon;
    bool canFlee;
};

struct game {
    struct gameMaster gm;
    struct card globalCardPool[DECK_SIZE];
    CardLink nodePool[DECK_SIZE];
    Zone mainDeck;
    Zone discardPile;
    CardLink* roomSlots[MAX_ROOM_SIZE];
    struct player playerOne;
};