#include "mainHeader.h"

void gameLoop(struct gameMaster* gm){
	struct game game = {0};
	gameSetUp(&game);		
		
		
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

bool isGameSessionActive(struct game* session){

return (session != NULL);

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
				printSessionsDeck(& (activeSession->maindeck));
                continue;
				
			case 2:
				printSessionDeck(& (activeSession->discardPile));
				continue;
				
            case 3: //Print a generated Test Deck
				printTestDeck();
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
		
};

void debugGenerateTempTestDeck(){
	struct deck testDeck = {0};
	generateDeck(&testDeck);
	printEntireDeck(&testDeck);
	pressEnterToContinue();
	
}


// Print each piece of data from the current card.
void printSessionsDeck(struct game* activeSession){
	
	if (isGameSessionActive(activeSession)){
        printEntireDeck(& (activeSession->mainDeck));
		
	} else {
					
		printf("ERROR: No active game session is currently running!\n");
					
	}
};






void printEntireDeckLoop(struct deck* deck ) {

	int deckSize = deck->deckSize;
    for(int incrementer = 0; incrementer < deckSize; incrementer++) {
		printf("Card ID: %d | Type: %c | Value: %d\n", 
            deck->cards[incrementer].id, 
            deck->cards[incrementer].type, 
            deck->cards[incrementer].cardValue);

}
	
	
	printf("--- END OF ARRAY ---\n");
};

void printCurrentPlayerStats(struct game* activeSession){
		
		printf(
			"Max HP: %d\nCurrent HP: %d\nWeapon Value %d\n",
			activeSession->playerOne.MaxHP, 
			activeSession->playerOne.hp, 
			activeSession->playerOne.weaponValue);
}

void printEntireDungeonRoomLoop(struct dungeon* dungeon) {
	
	int encounterSlots = dungeon->encounterCounter;
    for(int incrementer = 0; incrementer < encounterSlots; incrementer++) {
		printf("Dungeon Slot %d | Card ID: %d | Type: %c | Value: %d\n",
		incrementer,
		dungeon->card[incrementer].id,
		dungeon->card[incrementer].type,
		dungeon->card[incrementer].cardValue);
	}
	printf("--- END OF ARRAY ---\n");
}