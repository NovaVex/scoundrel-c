#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <iso646.h>

extern bool g_debugMenuEnabled;
extern int g_gameState

struct _card {
    int _id;
    char _type;
    int cardValue;
};

struct _player{
    int _hp;
    int weaponValue;
};

void randomNumberGenerator();

void generateDeck(struct _card mainDeck[]);

void startGame();

int userInputExstraction();

//Draw UI

int drawMainMenu();

int drawDebugMenu();