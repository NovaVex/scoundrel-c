#include "Game_mechanics.h"
#include "Data_Structure.h"
#include <stdlib.h>
#include <stdio.h>

CardLink* drawTopCard(Zone* pile) {

    if (pile->count == 0) return NULL;

    CardLink* drawnCard = pile->topCard;
    
    pile->topCard = drawnCard->next;
    pile->count--;
    drawnCard->nextCard = NULL; 
    
    if (pile->count == 0) pile->bottomCard = NULL;
    
    return drawnCard;
}

void placeAtBottom(Zone* pile, CardLink* card) {
    if (card == NULL) return;

    if (pile->count == 0) {
        pile->topCard = card;
        pile->bottomCard = card;
        pile->count++;
        return;
    }

    CardLink* currentBottom = (CardLink*)pile->bottomCard;
	
    currentBottom->nextCard = card;
    pile->bottomCard = card;
    
    pile->count++;
}

void generateCardPool(struct game* game, int* outTotalCards) {
    int cardValueGenerator = 2;
    int deckIncrementer; 

    // Chunk 1: Monsters
    for (deckIncrementer = 0; deckIncrementer < 26; deckIncrementer++) {
        if (cardValueGenerator == 15) cardValueGenerator = 2; 
        
        game->globalCardPool[deckIncrementer].id = deckIncrementer + 1;
        game->globalCardPool[deckIncrementer].type = MONSTER;
        game->globalCardPool[deckIncrementer].value = cardValueGenerator++;
    }

    cardValueGenerator = 2;

    // Chunk 2: Potions
    for (deckIncrementer = 26; deckIncrementer < 35; deckIncrementer++) {
        game->globalCardPool[deckIncrementer].id = deckIncrementer + 1;
        game->globalCardPool[deckIncrementer].type = POTION;
        game->globalCardPool[deckIncrementer].value = cardValueGenerator++;
    }

    cardValueGenerator = 2;

    // Chunk 3: Weapons
    for (deckIncrementer = 35; deckIncrementer < 44; deckIncrementer++) {
        game->globalCardPool[deckIncrementer].id = deckIncrementer + 1;
        game->globalCardPool[deckIncrementer].type = WEAPON;
        game->globalCardPool[deckIncrementer].value = cardValueGenerator++;
    }

    *outTotalCards = deckIncrementer;
}

void cardShuffle(CardLink** cardArray, int totalCards) {

    for (int currentSlot = totalCards - 1; currentSlot > 0; currentSlot--) {
        
        int randomSlot = rand() % (currentSlot + 1);

        CardLink* cardInHand = cardArray[currentSlot];
        cardArray[currentSlot] = cardArray[randomSlot];
        cardArray[randomSlot] = cardInHand;
    }
}

void buildDeck(struct game* game, int totalCards) {
    CardNode* shuffleArray[DECK_SIZE];
    
    for (int i = 0; i < totalCards; i++) {
        game->nodePool[i].cardRef = &game->globalCardPool[i];
        game->nodePool[i].next = NULL;
        shuffleArray[i] = &game->nodePool[i];
    }

    cardShuffle(shuffleArray, totalCards);

    game->mainDeck.head = shuffleArray[0];
    for (int i = 0; i < totalCards - 1; i++) {
        shuffleArray[i]->next = shuffleArray[i + 1];
    }
    shuffleArray[totalCards - 1]->next = NULL;

    game->mainDeck.tail = shuffleArray[totalCards - 1];
    game->mainDeck.count = totalCards;
}

void dealRoomCards(struct game* game) {
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {

        if (game->roomSlots[i] == NULL && game->mainDeck.count > 0) {
            game->roomSlots[i] = popTop(&game->mainDeck);
        }
    }
}

void fleeManager(struct game* game) {
    if (game->playerOne.canFlee == false) {
        printf("You can not flee\n");
        return; 
    }
    
    // Need
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        if (game->roomSlots[i] != NULL) {
            pushBottom(&game->mainDeck, game->roomSlots[i]);
            game->roomSlots[i] = NULL;
        }
    }
    
    setCanFleeFalse(&(game->playerOne));
}

void encounterManager(struct game* game, int encounterSlot) {
    if (game->roomSlots[encounterSlot] == NULL) {
        return;
    }

    CardNode* activeNode = game->roomSlots[encounterSlot];
    struct card* encounter = activeNode->cardRef;
    
    setCanFleeFalse(&(game->playerOne));
    
    switch(encounter->type) {
        case MONSTER:
            combatManager(&(game->playerOne), encounter);
            break;
            
        case POTION:
            healManager(&(game->playerOne), encounter);
            break;
            
        case WEAPON:
            equipWeapon(&(game->playerOne), encounter);
            break;
    }
    
    pushBottom(&game->discardPile, activeNode);
    game->roomSlots[encounterSlot] = NULL;
}

//still unsure how I want to do player death.

void combatManager(struct player* player, struct card* monster) {
    int damageDealt = 0;
    int playerHp = 0;
    
    damageDealt = decideDamageValue(player, monster);
    playerHp = clampedDamageToPlayer(player->hp, player->minHP, player->maxHP, damageDealt);
    setPlayerHealth(player, playerHp);
    
    bool playerDead = isPlayerDead(player);
}

void healManager(struct player* player, struct card* potion) {
    int healValue = potion->value;
    int playerHp = player->hp;
    
    playerHp = clampedPlayerHeal(playerHp, player->minHP, player->maxHP, healValue);
    setPlayerHealth(player, playerHp);
}

void equipWeapon(struct player* player, struct card* encounterWeapon) {
    player->weapon.killCount = 0;
    player->weapon.equipped = encounterWeapon; 
}

//Helpers 

bool isPlayerDead(struct player* player) {
    int playerHp = player->hp;
    if(playerHp <= 0) { return true; }    
    return false;
}

int decideDamageValue(struct player* player, struct card* monster) {
    int damageDealt = 0;
    WeaponState currentState = checkWeaponState(player, monster);
    
    switch(currentState) {
        case WEAPON_FRESH:
        case WEAPON_VALID_COMBO:
            damageDealt = player->weapon.equipped->value - monster->value;
            break;
            
        case WEAPON_NONE:
        case WEAPON_INVALID_COMBO:
            damageDealt = monster->value;
            break;
    }
    return damageDealt;
} 

WeaponState checkWeaponState(struct player* player, struct card* monster) {
    if (player->weapon.equipped == NULL || player->weapon.equipped->value == 0) {
        return WEAPON_NONE;
    }
    
    if (player->weapon.killCount == 0) {       
        return WEAPON_FRESH;
    }
    
    int lastKillIndex = player->weapon.killCount - 1;
    lastKillIndex = preventNegative(lastKillIndex);

    if (monster->value < player->weapon.monsterStack[lastKillIndex]->value) {
        return WEAPON_VALID_COMBO;
    }
    
    return WEAPON_INVALID_COMBO;
}

int clampedDamageToPlayer(int rawHp, int minHp, int maxHp, int rawDamageDealt) {
    rawDamageDealt = preventNegative(rawDamageDealt);
    int newHp = damageCalculation(rawHp, rawDamageDealt);
    return clamp(newHp, minHp, maxHp);
}

int damageCalculation(int currentHP, int damageTaken) {
    return currentHP - damageTaken;
}

int healCalculation(int currentHp, int healValue) {
    return currentHp + healValue;
}

int clampedPlayerHeal(int rawHp, int minHp, int maxHp, int rawHeal) {
    rawHeal = preventNegative(rawHeal);
    int newHp = healCalculation(rawHp, rawHeal);
    return clamp(newHp, minHp, maxHp);
}

void setPlayerHealth(struct player* player, int valueToSet) {
    player->hp = valueToSet;
}

int clamp(int value, int min, int max) {
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

int preventNegative(int value) {
    if(value < 0) return 0;
    return value;
}

void setCanFleeFalse(struct player* player) {
    player->canFlee = false;
}

void setCanFleeTrue(struct player* player) {
    player->canFlee = true;
}