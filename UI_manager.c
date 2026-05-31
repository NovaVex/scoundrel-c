#include "UI_manager.h"
#include "Data_Structure.h"
#include "Game_master.h"

#include <iso646.h>
#include <stdio.h>

void clearScreen(){

    printf("\033[H\033[J"); 
}

//debug tools

void printDungeonRoom(struct game* activeSession){
		
	if (isGameSessionActive(activeSession)){
		printf(
			"Max HP: %d\nCurrent HP: %d\nWeapon Value %d\n",
			activeSession->playerOne.maxHP, 
			activeSession->playerOne.hp, 
			activeSession->playerOne.weaponValue);
	
	pressEnterToContinue();
		
	} else {
					
		printf("ERROR: No active game session is currently running!\n");
					
		}
}

void printSessionDeck(struct game* activeSession){
	
	if (isGameSessionActive(activeSession)){
        printEntireDeckLoop(& (activeSession->mainDeck));
		
	} else {
					
		printf("ERROR: No active game session is currently running!\n");
					
	}
}

void printEntireDeckLoop(struct deck* deck ) {

	int deckSize = deck->deckSize;
    for(int incrementer = 0; incrementer < deckSize; incrementer++) {
		printf("Card ID: %d | Type: %c | Value: %d\n", 
            deck->cards[incrementer].id, 
            deck->cards[incrementer].type, 
            deck->cards[incrementer].cardValue);

}

	printf("--- END OF ARRAY ---\n");
}

void printCurrentPlayerStats(struct game* activeSession){
		
		printf(
			"Max HP: %d\nCurrent HP: %d\nWeapon Value %d\n",
			activeSession->playerOne.maxHP, 
			activeSession->playerOne.hp, 
			activeSession->playerOne.weaponValue);
}

void printEntireDungeonRoom(struct dungeon* dungeon) {
	
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

void debugRender(struct game* game) {
    printf("\n=== SCOUNDREL DEBUG VIEW ===\n");

    printf("HP: %d/20\n", game->player.hp);
    
    if (game->player.weaponValue > 0) {
        printf("Weapon Value: %d | Last Kill: %d\n", 
                game->player.weaponValue, game->player.lastKillValue);
    } else {
        printf("Weapon: [UNARMED]\n");
    }

    printf("Cards in Deck: %d\n", game->deck.remainingCards);
    printf("Can Flee Room? %s\n", game->state.canFlee ? "YES" : "NO");
    printf("----------------------------\n");
    
    for (int i = 0; i < 4; i++) {
        if (game->room[i].isActive) {

            printf("Slot %d: [%d of %c]\n", i + 1, game->room[i].value, game->room[i].suit);
        } else {
            printf("Slot %d: [EMPTY]\n", i + 1);
        }
    }
    printf("============================\n");
}