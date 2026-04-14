#include "Game_mechanics.h

void generateDeck(struct deck* generate){

    int cardValueGenerator = 2;
    int deckIncrmenter; 

    // Chunk 1: Monsters
    for (deckIncrmenter = 0; deckIncrmenter < 26; deckIncrmenter++) {
		
        if(cardValueGenerator  == 15 ){
			cardValueGenerator = 2;
			}
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1;
			generate->card[deckIncrmenter].type = 'M'; //Monsters
			generate->card[deckIncrmenter].cardValue = cardValueGenerator++;

}

    cardValueGenerator = 2;

    // Chunk 2: Potions
    for (deckIncrmenter = 26; deckIncrmenter < 35; deckIncrmenter++) {
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1;
			generate->card[deckIncrmenter].type = 'P'; //Potions
			generate->card[deckIncrmenter].cardValue = cardValueGenerator++;
    }

    cardValueGenerator = 2;

    // Chunk 3: Weapons
    for (deckIncrmenter = 35; deckIncrmenter < 44; deckIncrmenter++) {
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1;
			generate->card[deckIncrmenter].type = 'W'; //Weapons
			generate->card[deckIncrmenter].cardValue = cardValueGenerator++;
    }

		generate->deckSize = deckIncrmenter;
}

void shuffleDeck(struct deck* deck) { //Needs updated for struct
    for (int i = deckSize - 1; i > 0; i--) {

        int j = rand() % (i + 1);

        int tempDeck = deck[i];
        deck[i] = deck[j];
        deck[j] = tempDeck;
	}
}

