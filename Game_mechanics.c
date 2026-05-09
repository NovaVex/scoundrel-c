#include "Game_mechanics.h
#include "Data_Structure.h"

void generateDeck(struct deck* generate){

    int cardValueGenerator = 2;
    int deckIncrmenter; 

    // Chunk 1: Monsters
    for (deckIncrmenter = 0; deckIncrmenter < 26; deckIncrmenter++) {
		
        if(cardValueGenerator  == 15 ){
			cardValueGenerator = 2;
			}
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1;
			generate->card[deckIncrmenter].type = MONSTER
			generate->card[deckIncrmenter].value = cardValueGenerator++;

}

    cardValueGenerator = 2;

    // Chunk 2: Potions
    for (deckIncrmenter = 26; deckIncrmenter < 35; deckIncrmenter++) {
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1;
			generate->card[deckIncrmenter].type = POTION
			generate->card[deckIncrmenter].value = cardValueGenerator++;
    }

    cardValueGenerator = 2;

    // Chunk 3: Weapons
    for (deckIncrmenter = 35; deckIncrmenter < 44; deckIncrmenter++) {
		
            generate->card[deckIncrmenter].id = deckIncrmenter + 1;
			generate->card[deckIncrmenter].type = WEAPON
			generate->card[deckIncrmenter].value = cardValueGenerator++;
    }

		generate->deckSize = deckIncrmenter;
}

void shuffleDeck(struct deck* deck) {
	
	struct card* cards = deck->cards;
	int deckSize = deck->deckSize - 1;
	
    for (deckSize > 0; deckSize--) {

        int slotSelection = rand() % (deckSize + 1);

        struct card tempCard = cards[deckSize];
        cards[deckSize] = cards[slotSelection];
        cards[slotSelection] = tempDeck;
	}
}
    
void dealRoomCards(struct game* game) {
    
    int deckSize = game->mainDeck.deckSize;
    int deckSlotPosition = game->mainDeck.currentDeckSlot;
    int cardsRemaining = deckSize - deckSlotPosition;
    int remainingEncounters = game->dungeonRoom.encounterCounter;
    
    struct card* roomSlots = game->dungeonRoom.encounterSlots;
    int* roomCounter = &(game->dungeonRoom.encounterCounter);
    
    for (remainingEncounters; remainingEncounters < MAX_ROOM_SIZE; remainingEncounters++) {
        
        if (cardsRemaining > 0) {

            struct card* TopCard = &(game->mainDeck.cards[game->mainDeck.currentDeckSlot]);
            
            copyCard(TopCard, roomSlots, roomCounter);
            
            game->mainDeck.currentDeckSlot++;
            cardsRemaining--;
        } 
        else {

            roomSlots[remainingEncounters].type = EMPTY;
        }
    }
}

void encounterManager(struct game* game, int encounterSlot){
	
	struct card* encounter = &(game->dungeonRoom.encounterSlots[encounterSlot]);

	switch(encounter->type){
		case MONSTER:
			combatManager(game, encounterSlot);
			break;
			
		case POTION:
			healManager(game, encounterSlot);
			break;
			
		case WEAPON:
			equipWeapon(game, encounterSlot);
			break;
						
	}
	
	game->dungeonRoom.encounterCounter--;
}

//Need to possibly remove is player dead unsure how I want to handle this atm.
void combatManager(struct game* game, int encounterSlot){
	
	struct player* player = &(game->playerOne);
	struct card* monster = &(game->dungeonRoom.encounterSlots[encounterSlot]);
	struct card* discardPile = game->discardPile.cards;
	int* discardCounter = &(game->discardPile.deckSize);
	int damageDealt = 0;
	int playerHp = 0;
	bool playerDead = false;
	
	
	damageDealt = decideDamageValue(player, monster);
	
	playerHp = clampedDamageToPlayer(player->hp, player->minHp, player->maxHp, damageDealt);
		
	setPlayerHealth(player, playerHp);
	
	playerDead = isPlayerDead(player);
	
	copyCard(monster, discardPile, discardCounter);
	
	emptyCardSlot(monster);

}

bool isPlayerDead(struct player* player){
	
	int playerHp = player->hp;
	if(playerHp <= 0 ){return true;}	
	return false;
}

int decideDamageValue(struct player* player, struct card* monster){
	
	int damageDealt = 0;
	
	WeaponState currentState = checkWeaponState(player);
	
	switch(currentState){
	
		case WEAPON_FRESH:
		case WEAPON_VALID_COMBO:
		
			damageDealt = player->weapon.equipped.value - monster->value;
			break;
			
		case WEAPON_NONE:
		case WEAPON_INVALID_COMBO:
		
			damageDealt = monster->value;
			break;
}
	return damageDealt;
	
} 

WeaponState checkWeaponState(struct player* player, struct card* monster){
	
	if (player->weapon.equipped.value == 0){
		return WEAPON_NONE;
	}
	
	if (player->weapon.killCount == 0){		
		return WEAPON_FRESH;
	}
	
	int lastKillIndex = player->weapon.killCount - 1;
	lastKillIndex = preventNegative(lastKillIndex);
	
	if (monster->value < player->weapon.previousKill[lastKillIndex]){
		return WEAPON_VALID_COMBO;
	}
	
	return WEAPON_INVALID_COMBO;
}

int clampedDamageToPlayer(int rawHp, int minHp, int maxHp, int rawDamageDealt){
	
	rawDamageDealt = preventNegative(rawDamageDealt);
			
	int newHp = damageCalculation(rawHp, rawDamageDealt);
			
	return clamp(newHp, minHp, maxHp);
		
}

int damageCalculation(int currentHP, int damageTaken){
	return currentHP - damageTaken;
}

void healManager(struct game* game, int encounterSlot){
	
	struct player* player = &(game->playerOne);
	struct card* potion = &(game->dungeonRoom.encounterSlots[encounterSlot]);
	
	int healValue = potion->value;
	int playerHp = player->hp;
	
	playerHp = clampedPlayerHeal(playerHp, player->minHp, player->maxHp, potion);
	
	setPlayerHealth(player, playerHp);
	copyCard(potion, discardPile, discardCounter);
	emptyCardSlot(potion);
	
}

void equipWeapon(struct game* game, int encounterSlot){
	
	struct player* player = &(game->playerOne);
	struct card* encounterWeapon = &(game->dungeonRoom.encounterSlot[encounterSlot]);
	
	player->weapon.killCount = 0;
	player->weapon.equipped = *encounterWeapon;
	
	emptyCardSlot(encounterWeapon);
	
}

void copyCard(struct card* source, struct card* targetArray, int* targetCounter){
	
	int currentSlot = *targetCounter;
	targetArray[currentSlot] = *source;
	*targetCounter = *targetCounter + 1;
	
}

void emptyCardSlot(struct card* slotToClear){
	
	slotToClear->type = 'E';
    slotToClear->value = 0;
		
}

int healCalculation(int currentHp, int healValue){
	
	return currentHP = currentHP + healValue;
	
}

int clampedPlayerHeal(int rawHp, int minHp, int maxHp, int rawHeal){
	
	rawHeal = preventNegative(rawHeal);
	
	int newHp = healCalculation(rawHp, rawHeal);
	
	return clamp(newHp, minHp, maxHp);
	
	
}

void setPlayerHealth(struct player* player, int valueToSet){
	
	player->hp = valueToSet;
	
}

int clamp(int value, int min, int max){
	if(value < min) return min;
	
	if(value > max) return max;
	
	return value;
}

int preventNegative(int value){
	
	if(value < 0) return 0;
	return value;
}