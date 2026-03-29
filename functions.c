#include "mainHeader.h"

void wakeGameMaster(struct gameMaster* startingValues){
	
	startingValues->debugMenuEnabled = false;
	startingValues->debugOpen = false;
	startingValues->gameState = 1;
	randomNumberGenerator(startingValues);
}

void generateDeck(struct deck* generate){

    int cardValueGenerator = 2;
    int deckIncrmenter; 

    // Chunk 1: Monsters
    for (deckIncrmenter = 0; deckIncrmenter < 26; deckIncrmenter++) {
		
        if(cardValueGenerator  == 15 ){
			cardValueGenerator = 2;
			}
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1
			generate->card[deckIncrmenter].type = 'M' //Monsters
			generate->card[deckIncrmenter].cardValue = cardValueGenerator++

}

    cardValueGenerator = 2;

    // Chunk 2: Potions
    for (deckIncrmenter = 26; deckIncrmenter < 35; deckIncrmenter++) {
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1
			generate->card[deckIncrmenter].type = 'P' //Potions}
			generate->card[deckIncrmenter].cardValue = cardValueGenerator++
    }

    cardValueGenerator = 2;

    // Chunk 3: Weapons
    for (deckIncrmenter = 35; deckIncrmenter < 44; deckIncrmenter++) {
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1
			generate->card[deckIncrmenter].type = 'W' //Weapons
			generate->card[deckIncrmenter].cardValue = cardValueGenerator++
    }

		generate->deckSize = deckIncrmenter;
}

void setPlayerDefault(struct player*, defaultValues){
	
	defaultValues->maxHP = 20;
	defaultValues->hp = 20;
	defaultValues-> weaponValue = 0;
}

void shuffleDeck(int deck[], int deckSize) {
    for (int i = deckSize - 1; i > 0; i--) {

        int j = rand() % (i + 1);

        int tempDeck = deck[i];
        deck[i] = deck[j];
        deck[j] = tempDeck;
	}
}

void gameSetUp(struct game* session){
	setPlayerDefault(& (session->playerOne));	
	generateDeck(& (session->mainDeck));
	shuffleDeck(& (session->mainDeck));

}

void gameLoop(struct gameMaster* gm){
	struct game game = {0};
	gameSetUp(&game);		
		
		
	}
	
//Managers
int openMainMenu(){
		
	clearScreen();
	drawMainMenu();
	userInput = processUserInput();
	
	return userInput
	
	
}

void printSessionsDeck(struct game* activeSession){
	
	if (isGameSessionActive(activeSession)){
        printEntireDeck(& (activeSession->mainDeck));
		
	} else {
					
		printf("ERROR: No active game session is currently running!\n");
					
		}
}
	
void printTestDeck(){
	struct deck testDeck = {0};
	generateDeck(&testDeck);
	printEntireDeck(&testDeck);
	pressEnterToContinue();
	
}
	
void printCurrentPlayerStats(struct game* activeSession){
		
	if (isGameSessionActive(activeSession)){
		printf(
			"Max HP: %d\nCurrent HP: %d\nWeapon Value %d\n",
			activeSession->playerOne.MaxHP, 
			activeSession->playerOne.hp, 
			activeSession->playerOne.weaponValue);
	
	pressEnterToContinue();
		
	} else {
					
		printf("ERROR: No active game session is currently running!\n");
					
		}
}

void printDungeonRoom(struct game* activeSession){
		
	if (isGameSessionActive(activeSession)){
		printf(
			"Max HP: %d\nCurrent HP: %d\nWeapon Value %d\n",
			activeSession->playerOne.MaxHP, 
			activeSession->playerOne.hp, 
			activeSession->playerOne.weaponValue);
	
	pressEnterToContinue();
		
	} else {
					
		printf("ERROR: No active game session is currently running!\n");
					
		}
}

}	
	

//debug tools

void openDebugMenu(struct gameMaster* engine, struct game* activeSession){
    if (not debugMenuEnabled) {
        return; 
    }
	engine->debugOpen = true;
	
    int playerChoice;
	
    while(engine->debugOpen = true) {
        drawDebugMenu(); 
        
        playerChoice = processUserInput;

        switch (playerChoice) {

            case 1://Print currently Active deck
				printSessionsDeck(activeSession);
                continue;

            case 2: //Print a generated Test Deck
				printTestDeck();
                continue;
				
			case 3:
				printCurrentPlayerStats;
				continue;
			
            case 9://Close Menu
				engine->debugOpen = false;
                return; 
                
            default:
                break;
        }
		
    }

}




// Print each piece of data from the current card.


void printEntireDeck(struct card deck[], int deckSize) {

    for(int incrementer = 0; i < deckSize; incrementer++) {
		printf("Card ID: %d | Type: %c | Value: %d\n", 
            deck[incrementer].id, 
            deck[incrementer].type, 
            deck[incrementer].cardValue);

	}
	printf("--- END OF ARRAY ---\n");
}

void printEntireDungeonRoom(struct dungeon, int encounterSlots) {

    for(int incrementer = 0; i < encounterSlots; incrementer++) {


	}
	printf("--- END OF ARRAY ---\n");