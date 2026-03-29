#include "mainHeader.h"


int processUserInput(){// possibly need to redo. Doing too many things at once.

    char rawUserInput[21];
    int playerChoice = -1;

    scanf("%20s", rawUserInput);

    if (isalpha(rawUserInput[0]))
    {
        if (strcmp(rawUserInput, "debug") == 0)
        {
            g_debugMenuEnabled = not g_debugMenuEnabled;
            printf("Debug menu enabled.\n");
            return 0;
        }
        
        else
        {
            printf("Unknown command. Please use 1-9 to make a selection.\n")
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

void randomNumberGenerator(struct gameMaster* engine){

	unsigned int startingSeed = (unsigned int)time(NULL);

    srand(startingSeed);

    int rngToSkip = rand() % 100;

    for (int rngIncrimenter = 0; rngIncrimenter < rngToSkip; rngIncrimenter++){
        rand();
    }

	engine->rngSeed = startingSeed;
}

void clearScreen(){

    printf("\033[H\033[J"); 
}

void pressEnterToContinue(){
    int c;
    
    while ((c = getchar()) != '\n' && c != EOF) { } 
    
    printf("\nPress [Enter] to continue...");
    
    getchar(); 
}

bool isGameSessionActive(struct activeGameSession* session){

return (session != NULL);

}