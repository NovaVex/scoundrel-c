#include "Data_Structure.h"
#include "scene_manager.h"
#include "Game_Loop.c"


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
};

void wakeGameMaster(struct gameMaster* startingValues){
	
	startingValues->debugMenuEnabled = false;
	startingValues->debugOpen = false;
	startingValues->gameState = 1;
	randomNumberGenerator(startingValues);
};