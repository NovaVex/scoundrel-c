#include "scene_manager.h"

#include "Data_Structure.h"
#include "Assets.h"
#include "Game_mechanics.h"
#include "UI_manager.h"
#include "input.h"
#include <stdio.h>
#include <stdbool.h>
#include <iso646.h>


int openMainMenu(){

	clearScreen();
	drawMainMenu();
	int userInput = processUserInput();

	return userInput;

}

void openDebugMenu(struct gameMaster* engine, struct game* activeSession){
    if (not engine->debugMenuEnabled) {
        return; 
    }
	engine->debugOpen = true;
	
    int playerChoice;
	
    while(engine->debugOpen == true) {
        drawDebugMenu(); 
        
        playerChoice = processUserInput();

        switch (playerChoice) {

            case 1://Print currently Active deck
				printSessionsDeck(& (activeSession->mainDeck));
                continue;
				
			case 2:
				printSessionDeck(& (activeSession->discardPile));
				continue;
				
            case 3: //Print a generated Test Deck
				debugGenerateTempTestDeck();
                continue;
				
			case 4:
				printCurrentPlayerStats(& (activeSession->playerOne));
				continue;
				
			case 5:
				printDungeonRoom(& (activeSession->dungeonRoom));
				continue;
				
            case 9://Close Menu
				engine->debugOpen = false;
                return; 
                
            default:
                break;
        }
		
	}
}

void debugGenerateTempTestDeck(){
	struct deck testDeck = {0};
	generateDeck(&testDeck);
	printEntireDeck(&testDeck);
	pressEnterToContinue();
	
}