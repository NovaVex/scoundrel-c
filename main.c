#include <stdio.h>
#include "mainHeader.h"

int main() {

    struct _card mainDeck[44];
    int playerChoice;
    bool g_debugMenuEnabled = false;
    int g_gameState = 1;

    randomNumberGenerator();
    generateDeck(mainDeck);

    while(1){

        switch(g_gameState) 
        {
            case 1:
                
            playerChoice == drawMainMenu();

                switch (playerChoice)
                {
                    case 1:
                    g_gameState = 2 //GameStarted
                    break;

                    case 2:
                    g_gameState = 3 //Options
                    break;

                    case 3:
                    return 0

                    case 4:
                    return 0;

                    default:
                    break;
                }

            case 2:
                startGame();
                break;
	        case 3:
                break;
                
            case 9:
                debugMenu();
                continue;
        }
    }
return 0;
}