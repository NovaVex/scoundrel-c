#include <stdio.h>
#include "mainHeader.h"

int main() {
	struct gameMaster gameMaster = {0};
	wakeGameMaster;
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