#include "Game_mechanics.h"
#include "Data_Structure.h"
#include <stdlib.h>
#include <stdio.h>

// ========================================================
// Card Manipulation 
// ========================================================
CardLink* drawTopCard(Zone* pile) {
    if (pile->count == 0) return NULL;

    CardLink* drawnCard = pile->topCard;
    
    pile->topCard = drawnCard->next;
    pile->count--;
    drawnCard->next = NULL; 
    
    if (pile->count == 0) pile->bottomCard = NULL;
    
    return drawnCard;
}

void placeAtBottom(Zone* pile, CardLink* card) {
    if (card == NULL) return;

    if (pile->count == 0) {
        pile->topCard = card;
        pile->bottomCard = (void*)card;
        pile->count++;
        return;
    }

    CardLink* currentBottom = (CardLink*)pile->bottomCard;
    
    currentBottom->next = card;
    pile->bottomCard = (void*)card;
    
    pile->count++;
}

// ========================================================
// Builders
// ========================================================
void generateCardPool(struct game* game, int* outTotalCards) {
    int cardValueGenerator = 2;
    int deckIncrementer; 

    for (deckIncrementer = 0; deckIncrementer < 26; deckIncrementer++) {
        if (cardValueGenerator == 15) cardValueGenerator = 2; 
        
        game->globalCardPool[deckIncrementer].id = deckIncrementer + 1;
        game->globalCardPool[deckIncrementer].type = MONSTER;
        game->globalCardPool[deckIncrementer].value = cardValueGenerator++;
    }

    cardValueGenerator = 2;

    for (deckIncrementer = 26; deckIncrementer < 35; deckIncrementer++) {
        game->globalCardPool[deckIncrementer].id = deckIncrementer + 1;
        game->globalCardPool[deckIncrementer].type = POTION;
        game->globalCardPool[deckIncrementer].value = cardValueGenerator++;
    }

    cardValueGenerator = 2;

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
    CardLink* shuffleArray[DECK_SIZE];
    
    for (int currentSlot = 0; currentSlot < totalCards; currentSlot++) {
        CardLink* currentLink = (CardLink*)&game->nodePool[currentSlot];
        
        currentLink->data = &game->globalCardPool[currentSlot];
        currentLink->next = NULL;
        
        shuffleArray[currentSlot] = currentLink;
    }

    cardShuffle(shuffleArray, totalCards);

    game->mainDeck.topCard = shuffleArray[0];
    
    for (int currentSlot = 0; currentSlot < totalCards - 1; currentSlot++) {
        CardLink* currentCard = shuffleArray[currentSlot];
        CardLink* cardDirectlyBeneath = shuffleArray[currentSlot + 1];
        
        currentCard->next = cardDirectlyBeneath;
    }
    
    int lastCardIndex = totalCards - 1;
    shuffleArray[lastCardIndex]->next = NULL;

    game->mainDeck.bottomCard = (void*)shuffleArray[lastCardIndex];
    game->mainDeck.count = totalCards;
}

// ========================================================
// Managers
// ========================================================
void dealRoomCards(struct game* game) {
    Zone* deck = &game->mainDeck;
    
    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        if (deck->count == 0) return;

        if (isRoomSlotEmpty(game, roomSlot)) {
            CardLink* drawnCard = drawTopCard(deck);
            game->roomSlots[roomSlot] = (void*)drawnCard;
        }
    }
}

void fleeManager(struct game* game) {
    if (game->playerOne.canFlee == false) {
        return; 
    }
    
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        if (!isRoomSlotEmpty(game, i)) {
            placeAtBottom(&game->mainDeck, (CardLink*)game->roomSlots[i]);
            game->roomSlots[i] = NULL;
        }
    }
    
    setCanFleeFalse(&(game->playerOne));
    
    dealRoomCards(game);
}

void encounterManager(struct game* game, int chosenSlot) {
    if (isRoomSlotEmpty(game, chosenSlot)) return;

    CardLink* cardLinkOnTable = (CardLink*)game->roomSlots[chosenSlot];
    struct card* actualCard = cardLinkOnTable->data;
    
    struct player* player = &game->playerOne;
    Zone* discardPile = &game->discardPile;
    
    setCanFleeFalse(player);
    
    switch (actualCard->type) {
        case MONSTER:
            combatManager(player, actualCard);
            break;
            
        case POTION:
            healManager(player, actualCard);
            break;
            
        case WEAPON:
            equipWeapon(player, actualCard);
            break;
    }
    
    placeAtBottom(discardPile, cardLinkOnTable);
    game->roomSlots[chosenSlot] = NULL; 
}

void combatManager(struct player* player, struct card* monster) {
	
    WeaponState currentState = checkWeaponState(player, monster);
    
    int damageTaken = decideDamageValue(player, monster);
    int newHp = clampedDamageToPlayer(player->hp, player->minHP, player->maxHP, damageTaken);
    setPlayerHealth(player, newHp);
    
    if (currentState == WEAPON_FRESH || currentState == WEAPON_VALID_COMBO) {
        int killIndex = player->weapon.killCount;
        
        if (killIndex < MAX_MONSTER_WEAPON_STACK) {
            player->weapon.monsterStack[killIndex] = monster;
            player->weapon.killCount++;
        }
    }
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

// ========================================================
// Helpers
// ========================================================
bool isRoomSlotEmpty(struct game* game, int slotIndex) {
    if (game->roomSlots[slotIndex] == NULL) {
        return true;
    }
    return false;
}

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

            damageDealt = monster->value - player->weapon.equipped->value;
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

int countCardsInRoom(struct game* game) {
    int count = 0;
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        if (!isRoomSlotEmpty(game, i)) {
            count++;
        }
    }
    return count;
}