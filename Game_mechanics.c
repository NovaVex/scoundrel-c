#include "Game_mechanics.h"
#include "Data_Structure.h"
#include <stdlib.h>

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

void placeAtBottom(Zone* pile, CardLink* cardToPlace) {
    if (cardToPlace == NULL) return;

    cardToPlace->next = NULL;

    if (pile->count == 0) {
        pile->topCard = cardToPlace;
        pile->bottomCard = cardToPlace;
        pile->count++;
        return;
    }

    pile->bottomCard->next = cardToPlace;
    pile->bottomCard = cardToPlace;
    pile->count++;
}

// ========================================================
// Builders
// ========================================================
void generateCardPool(struct game* session, int* outTotalCards) {
    int cardValueGenerator = 2;
    int deckIncrementer;

    for (deckIncrementer = 0; deckIncrementer < 26; deckIncrementer++) {
        if (cardValueGenerator == 15) cardValueGenerator = 2;

        session->globalCardPool[deckIncrementer].identifier = deckIncrementer + 1;
        session->globalCardPool[deckIncrementer].type = MONSTER;
        session->globalCardPool[deckIncrementer].value = cardValueGenerator++;
    }

    cardValueGenerator = 2;
    for (deckIncrementer = 26; deckIncrementer < 35; deckIncrementer++) {
        session->globalCardPool[deckIncrementer].identifier = deckIncrementer + 1;
        session->globalCardPool[deckIncrementer].type = POTION;
        session->globalCardPool[deckIncrementer].value = cardValueGenerator++;
    }

    cardValueGenerator = 2;
    for (deckIncrementer = 35; deckIncrementer < 44; deckIncrementer++) {
        session->globalCardPool[deckIncrementer].identifier = deckIncrementer + 1;
        session->globalCardPool[deckIncrementer].type = WEAPON;
        session->globalCardPool[deckIncrementer].value = cardValueGenerator++;
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

void buildDeck(struct game* session, int totalCards) {
    CardLink* shuffleArray[DECK_SIZE];

    for (int currentSlot = 0; currentSlot < totalCards; currentSlot++) {
        CardLink* currentLink = &session->nodePool[currentSlot];

        currentLink->data = &session->globalCardPool[currentSlot];
        currentLink->next = NULL;

        shuffleArray[currentSlot] = currentLink;
    }

    cardShuffle(shuffleArray, totalCards);

    for (int currentSlot = 0; currentSlot < totalCards - 1; currentSlot++) {
        shuffleArray[currentSlot]->next = shuffleArray[currentSlot + 1];
    }

    int lastCardIndex = totalCards - 1;
    shuffleArray[lastCardIndex]->next = NULL;

    session->mainDeck.topCard = shuffleArray[0];
    session->mainDeck.bottomCard = shuffleArray[lastCardIndex];
    session->mainDeck.count = totalCards;
}

// ========================================================
// Managers
// ========================================================
void dealRoomCards(struct game* session) {
    Zone* deck = &session->mainDeck;

    session->playerOne.potionUsedThisRoom = false;

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        if (deck->count == 0) return;
        if (isRoomSlotEmpty(session, roomSlot)) {
            session->roomSlots[roomSlot] = drawTopCard(deck);
        }
    }
}

bool fleeManager(struct game* session) {
    if (session->playerOne.canFlee == false) return false;

    for (int slotIndex = 0; slotIndex < MAX_ROOM_SIZE; slotIndex++) {
        if (isRoomSlotEmpty(session, slotIndex)) continue;

        placeAtBottom(&session->mainDeck, session->roomSlots[slotIndex]);
        session->roomSlots[slotIndex] = NULL;
    }

    setCanFleeFalse(&session->playerOne);

    dealRoomCards(session);
    return true;
}

EncounterResult encounterManager(struct game* session, int chosenSlot, bool fightBarehanded) {
    if (isRoomSlotEmpty(session, chosenSlot)) return ENCOUNTER_NOTHING;

    CardLink* facedLink = session->roomSlots[chosenSlot];
    struct card* facedCard = facedLink->data;
    struct player* activePlayer = &session->playerOne;

    setCanFleeFalse(activePlayer);

    EncounterResult result = ENCOUNTER_NOTHING;

    switch (facedCard->type) {
        case MONSTER:
            combatManager(activePlayer, facedCard, fightBarehanded);
            result = ENCOUNTER_MONSTER_FOUGHT;
            break;

        case POTION:
            result = healManager(activePlayer, facedCard);
            break;

        case WEAPON:
            equipWeapon(activePlayer, facedCard);
            result = ENCOUNTER_WEAPON_EQUIPPED;
            break;
    }

    session->lastFacedType = facedCard->type;
    session->lastFacedValue = facedCard->value;

    placeAtBottom(&session->discardPile, facedLink);
    session->roomSlots[chosenSlot] = NULL;

    return result;
}

void combatManager(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded) {
    applyMonsterDamage(activePlayer, monsterCard, fightBarehanded);

    if (weaponWasUsed(activePlayer, monsterCard, fightBarehanded)) {
        stackMonsterOnWeapon(activePlayer, monsterCard);
    }
}

EncounterResult healManager(struct player* activePlayer, struct card* potionCard) {
    if (activePlayer->potionUsedThisRoom) return ENCOUNTER_POTION_FIZZLED;

    int healedHealth = clampedPlayerHeal(
        activePlayer->currentHealth,
        activePlayer->minimumHealth,
        activePlayer->maximumHealth,
        potionCard->value);

    setPlayerHealth(activePlayer, healedHealth);
    activePlayer->potionUsedThisRoom = true;

    return ENCOUNTER_POTION_HEALED;
}

void equipWeapon(struct player* activePlayer, struct card* weaponCard) {
    
    activePlayer->weapon.killCount = 0;
    activePlayer->weapon.equippedCard = weaponCard;
}

// ========================================================
// Combat helpers
// ========================================================
void applyMonsterDamage(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded) {
    int damageTaken = decideDamageValue(activePlayer, monsterCard, fightBarehanded);

    int newHealth = clampedDamageToPlayer(
        activePlayer->currentHealth,
        activePlayer->minimumHealth,
        activePlayer->maximumHealth,
        damageTaken);

    setPlayerHealth(activePlayer, newHealth);
}

void stackMonsterOnWeapon(struct player* activePlayer, struct card* monsterCard) {
    int killIndex = activePlayer->weapon.killCount;

    if (killIndex >= MAX_MONSTER_WEAPON_STACK) return;

    activePlayer->weapon.monsterStack[killIndex] = monsterCard;
    activePlayer->weapon.killCount++;
}

bool weaponWasUsed(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded) {
    if (fightBarehanded) return false;
    return weaponIsUsableAgainst(activePlayer, monsterCard);
}

bool weaponIsUsableAgainst(struct player* activePlayer, struct card* monsterCard) {
    WeaponState state = checkWeaponState(activePlayer, monsterCard);

    if (state == WEAPON_FRESH) return true;
    if (state == WEAPON_VALID_COMBO) return true;
    return false;
}

int decideDamageValue(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded) {
    
    if (fightBarehanded) return monsterCard->value;

    if (weaponIsUsableAgainst(activePlayer, monsterCard)) {
        return monsterCard->value - activePlayer->weapon.equippedCard->value;
    }

    return monsterCard->value;
}

WeaponState checkWeaponState(struct player* activePlayer, struct card* monsterCard) {
    if (activePlayer->weapon.equippedCard == NULL) return WEAPON_NONE;
    if (activePlayer->weapon.equippedCard->value == 0) return WEAPON_NONE;
    if (activePlayer->weapon.killCount == 0) return WEAPON_FRESH;

    int lastKillIndex = preventNegative(activePlayer->weapon.killCount - 1);
    int lastKillValue = activePlayer->weapon.monsterStack[lastKillIndex]->value;

    if (monsterCard->value <= lastKillValue) return WEAPON_VALID_COMBO;

    return WEAPON_INVALID_COMBO;
}

// ========================================================
// Rule queries
// ========================================================
bool isRoomSlotEmpty(struct game* session, int slotIndex) {
    return (session->roomSlots[slotIndex] == NULL);
}

bool isPlayerDead(struct player* activePlayer) {
    return (activePlayer->currentHealth <= 0);
}

bool isDungeonCleared(struct game* session) {
    if (session->mainDeck.count > 0) return false;
    return (countCardsInRoom(session) == 0);
}

bool mustKeepLastCardForNextRoom(struct game* session) {
    if (session->mainDeck.count == 0) return false;
    return (countCardsInRoom(session) <= 1);
}

bool roomIsReadyToLeave(struct game* session) {
    return (countCardsInRoom(session) <= 1);
}

int countCardsInRoom(struct game* session) {
    int count = 0;

    for (int slotIndex = 0; slotIndex < MAX_ROOM_SIZE; slotIndex++) {
        if (isRoomSlotEmpty(session, slotIndex)) continue;
        count++;
    }

    return count;
}

int calculateFinalScore(struct game* session) {
    struct player* activePlayer = &session->playerOne;

    if (isPlayerDead(activePlayer)) {
        int score = activePlayer->currentHealth;

        CardLink* currentLink = session->mainDeck.topCard;
        while (currentLink != NULL) {
            if (currentLink->data->type == MONSTER) score -= currentLink->data->value;
            currentLink = currentLink->next;
        }

        for (int slotIndex = 0; slotIndex < MAX_ROOM_SIZE; slotIndex++) {
            if (isRoomSlotEmpty(session, slotIndex)) continue;
            if (session->roomSlots[slotIndex]->data->type != MONSTER) continue;

            score -= session->roomSlots[slotIndex]->data->value;
        }

        return score;
    }

    bool atFullHealth = (activePlayer->currentHealth == activePlayer->maximumHealth);
    bool lastCardWasPotion = (session->lastFacedType == POTION);

    if (atFullHealth && lastCardWasPotion) {
        return activePlayer->currentHealth + session->lastFacedValue;
    }

    return activePlayer->currentHealth;
}

// ========================================================
// Health math
// ========================================================
int clampedDamageToPlayer(int rawHealth, int minimumHealth, int maximumHealth, int rawDamage) {
    rawDamage = preventNegative(rawDamage);
    int newHealth = damageCalculation(rawHealth, rawDamage);
    return clamp(newHealth, minimumHealth, maximumHealth);
}

int clampedPlayerHeal(int rawHealth, int minimumHealth, int maximumHealth, int rawHeal) {
    rawHeal = preventNegative(rawHeal);
    int newHealth = healCalculation(rawHealth, rawHeal);
    return clamp(newHealth, minimumHealth, maximumHealth);
}

int damageCalculation(int currentHealth, int damageTaken) {
    return currentHealth - damageTaken;
}

int healCalculation(int currentHealth, int healValue) {
    return currentHealth + healValue;
}

void setPlayerHealth(struct player* activePlayer, int valueToSet) {
    activePlayer->currentHealth = valueToSet;
}

int clamp(int value, int minimum, int maximum) {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

int preventNegative(int value) {
    if (value < 0) return 0;
    return value;
}

// ========================================================
// Flag setters
// ========================================================
void setCanFleeFalse(struct player* activePlayer) {
    activePlayer->canFlee = false;
}

void setCanFleeTrue(struct player* activePlayer) {
    activePlayer->canFlee = true;
}
