#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <iso646.h>

#define MAXDECKSIZE 50
#define DUNGEONSLOTS 4
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
	struct card[13];
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

//User Actions
void startGame();


//Hidden Mechanics

void randomNumberGenerator(struct gameMaster* engine);

void setPlayerDefault(struct player*, defaultValues);

void shuffleDeck(int deck[], int deckSize);

void generateDeck(struct deck* generate);

void setPlayerDefault(struct player*, defaultValues);

void gameSetUp(struct game* session);

void gameLoop(struct gameMaster* gm);

//UI Drawing
void clearScreen(); //Inside of draw menus

void drawMainMenu();

void drawDebugMenu();


//UI Managers

int openMainMenu();

void openDebugMenu(struct coreEngineVariables* coreEngine);

void wakeGameMaster(struct gameMaster* startingValues);

//Input
void pressEnterToContinue();

int processUserInput();

//Debug
void openDebugMenu();