#pragma once
#include <stdbool.h>

#define MAX_DECK_SIZE 50
#define MAX_ROOM_SIZE 4
struct gameMaster{

	bool debugMenuEnabled;
	bool debugOpen;
    int gameState;
	int rngSeed;
	//int screenWidth
	//int screenHeight
	
};

struct card{

    int id;
    char type;
    int value;
};

struct deck{
	
	int currentDeckSlot;
	int deckSize;
	struct card cards[MAX_DECK_SIZE];
};

struct weapon{
	
	struct card 	equipped;
	struct card 	monsterStack[13];
	int killCount;
};

struct player{
	int minHP;
	int maxHP;
    int hp;
    struct weapon	weapon;
};

struct dungeon{

	int encounterCounter;
	struct card 	encounterSlots[4];
};

struct game{

	struct player 		playerOne;
	struct deck			mainDeck;
	struct dungeon 		dungeonRoom;
	struct deck 		discardPile;
};
//Remember to move these to where they actually belong
typedef enum{
    WEAPON_NONE,           
    WEAPON_FRESH,          
    WEAPON_VALID_COMBO,    
    WEAPON_INVALID_COMBO   
} WeaponState;

typedef enum{
	EMPTY = 'E',
	MONSTER = 'M',
	POTION = 'P',
	WEAPON = 'W'
} CardType;

typedef enum {
	PLAYING_ACTIVE,
    PLAYING_PAUSED,
    PLAYING_OPTIONS,
    PLAYING_GAMEOVER,
    PLAYING_EXIT
} InGameState;