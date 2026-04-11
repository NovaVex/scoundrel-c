#pragma once
#include <stdbool.h>

#define MAX_DECK_SIZE 50
struct gameMaster{

	bool debugMenuEnabled;
	bool debugOpen;
    int gameState;
	int rngSeed;
};

struct card {

    int id;
    char type;
    int cardValue;
};

struct deck{

	int deckSize;
	struct card[MAX_DECK_SIZE];
};

struct weaponStack{
	
	int stackSize;
	struct card weapons[13];
}

struct player{

	int maxHP;
    int hp;
    int weaponValue;
};

struct dungeon{

	int encounterCounter;
	struct card encounterSlots[4];
};

struct game {

	struct player 		playerOne;
	struct deck			mainDeck;
	struct dungeon 		dungeonRoom;
	struct weaponStack 	weaponStack;
	struct deck 		discardPile;
};

