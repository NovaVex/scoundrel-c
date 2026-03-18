#include "mainHeader.h"

void generateDeck(struct _card mainDeck[]) 
{

    // Single Characters used for type for ASCII Rendering Tree.
    int cardValueGenerator = 2;
    int deckIncrmenter; 

    // Chunk 1: Monsters
    for (deckIncrmenter = 0; deckIncrmenter < 26; deckIncrmenter++) {
        if(cardValueGenerator  == 15 ){
            cardValueGenerator = 2;
        }
            mainDeck[deckIncrmenter]._id = deckIncrmenter + 1;
            mainDeck[deckIncrmenter]._type = "M"; //M = Monsters
            mainDeck[deckIncrmenter].cardValue = cardValueGenerator++;
}

    cardValueGenerator = 2;

    // Chunk 2: Potions
    for (deckIncrmenter = 26; deckIncrmenter < 35; deckIncrmenter++) {
        mainDeck[deckIncrmenter]._id = deckIncrmenter + 1; 
        mainDeck[deckIncrmenter]._type = "P"; //P = Potions
        mainDeck[deckIncrmenter].cardValue = cardValueGenerator++;
    }

    cardValueGenerator = 2;

    // Chunk 3: Weapons
    for (deckIncrmenter = 35; deckIncrmenter < 44; deckIncrmenter++) {
        mainDeck[deckIncrmenter]._id = deckIncrmenter + 1; 
        mainDeck[deckIncrmenter]._type = "W"; //W = Weapons
        mainDeck[deckIncrmenter].cardValue = cardValueGenerator++;
    }
}

void randomNumberGenerator()
{

    srand(time(NULL));
    int rngToSkip = rand() % 100;

    for (int rngIncrimenter = 0; rngIncrimenter < rngToSkip; rngIncrimenter++){
        rand();
    }

}

int userInputExstraction()
{
    char rawUserInput[21];
    int playerChoice = -1;

    scanf("%20s", rawUserInput);

    if (isalpha(rawUserInput[0]))
    {
        if (strcmp(rawUserInput, "debug") == 0)
        {
            g_debugMenuEnabled = not g_debugMenuEnabled;
            printf("Debug Enabled.\n");
            return 0;
        }
        
        else
        {
            printf("Unknown command.\n")
            return 0;
        }
        
    }
    else
    {
        if (isdigit(rawUserInput[0]))
        {
        playerChoice = atoi(rawUserInput);
        }
    
        else
        {
            printf("Unknown command, please use 1-9 to make a selection.");
            return 0;
        }
    }

    return playerChoice;

}

void startGame()
{



}

//debug tools

void debugMenu()
{

    if (g_debugMenuEnabled == true)
    {
    
        playerChoice = drawDebugMenu();

    
            switch (playerChoice)
            {
            case 1:
                printCards();
                break;

            case 2:
                
            
            default:
                break;
            }

            else 

    }
}

// Print each piece of data from the current card.
void printCards(){

    for(int i = 0; i < 44; i++) 
    {
        printf("Card ID: %d | Type: %c | Value: %d\n", 
            mainDeck[i]._id, 
            mainDeck[i]._type, 
            mainDeck[i].cardValue);
    }

}


