#include "Game_master.h"

#include <stdlib.h>
#include <time.h>
#include "Data_Structure.h"
#include "Scene_manager.h"
#include "Game_mechanics.c"

void gameMaster(){
	struct gameMaster gameMaster = {0};
	wakeGameMaster(&gameMaster);

while(gameMaster.gameState >= 1){

        switch(gameMaster.gameState) 
        {
            case 1:
                
            playerChoice = openMainMenu();

                switch (playerChoice)
                {
                    case 1:
                    gameMaster.gameState = 2; //Start Game
                    break;

                    case 2:
                    gameMaster.gameState = 3; //Options
                    break;

					case 9:
					debugMenu(&gameMaster, NULL);
					break;

                    default:
                    break;
                }
			
				break;

            case 2:
                gameLoop(&gameMaster);
                break;

	        case 3:
                break;
                
            case 9:
                debugMenu(&gameMaster);
                continue;
        }
    }
	return 0;
}

void wakeGameMaster(struct gameMaster* startingValues){
	
	startingValues->debugMenuEnabled = false;
	startingValues->debugOpen = false;
	startingValues->gameState = 1;
	randomNumberGenerator(startingValues);
}

void gameLoop(struct gameMaster* gm){
	struct game session = {0};
	gameSetUp(&session);		
	InGameState currentGameState = PLAYING_ACTIVE;
	
	while(currentGameState != PLAYING_EXIT);{
		
		switch(currentGameState){
			
			case PLAYING_ACTIVE:
				currentGameState = activeGameManger(&session, gm);
				break;
				
			case PLAYING_PAUSED:
				currentGameState = activeGamePauseManger(&session, gm);
				break;
				
			case PLAYING_OPTIONS:
				currentGameState = activeGameOptionsManger(&session, gm);
				break;
				
			case PLAYING_GAMEOVER:
				currentGameState = activeGameOverManger(&session, gm);
				break;
				
			case PLAYING_EXIT:
			default
			//to do error handling
			currentGameState = PLAYING_EXIT;
			break;
		}
		
	}
}

InGameState activeGameManger(struct game*, struct gameMaster*){
	
}

InGameState activeGamePauseManger(struct game*, struct gameMaster*){
	
}

InGameState activeGameOptionsManger(struct game*, struct gameMaster*){
	
}

InGameState activeGameOverManger(struct game*, struct gameMaster*){
	
}

void gameSetUp(struct game* session){
	setPlayerDefault(& (session->playerOne));	
	generateDeck(& (session->mainDeck));
	shuffleDeck(& (session->mainDeck));

}

void setPlayerDefault(struct player* defaultValues){
	defaultValues->minHp = 0;
	defaultValues->maxHP = 20;
	defaultValues->hp = 20;
	defaultValues->weapon.equipped = 0;
	defaultValues->canFlee = true;
}

bool isGameSessionActive(struct game* session){

return (session != NULL);

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