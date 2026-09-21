// ========================================================
// Game_mechanics.c
// --------------------------------------------------------
// The rules, and the only file that changes game state.
// No stdio: it returns values and the layers above print.
// That keeps it portable into a game engine later.
//
// Card Manipulation, Builders, Managers, Scoring, then Helpers
// (Questions, Combat, Previews, Health, Player and room state).
// ========================================================

#include "Game_mechanics.h"
#include <stdlib.h>

// ========================================================
// Card Manipulation
// ========================================================
//OK
Card* drawTopCard(Zone* pile) {
    if (pile->count == 0) return NULL;

    Card* drawnCard = cardAtPosition(pile, 0);

    pile->cards[pile->topIndex] = NULL;
    pile->topIndex = (pile->topIndex + 1) % DECK_SIZE;   // step down one slot, wrapping back to 0 at DECK_SIZE
    pile->count--;

    return drawnCard;
}

//OK
void placeAtBottom(Zone* pile, Card* card) {
    if (card == NULL) return;
    if (pile->count == DECK_SIZE) return;

    int bottomSlot = (pile->topIndex + pile->count) % DECK_SIZE;   // count slots past the top, wrapping at DECK_SIZE

    pile->cards[bottomSlot] = card;
    pile->count++;
}

//Grabs and returns the card at the position requested.
Card* cardAtPosition(Zone* pile, int position) {
    if (position < 0 || position >= pile->count) return NULL;      // || means OR: before the top, or past the bottom

    int ringSlot = (pile->topIndex + position) % DECK_SIZE;         // position slots past the top, wrapping at DECK_SIZE

    return pile->cards[ringSlot];
}

// ========================================================
// Builders
// ========================================================

// Builds the Global Card Pool and returns the total cards created.
int generateGlobalCardPool(Card* cardPool) {
    int totalCards = 0;

    generateEncounter(cardPool, MONSTER, MIN_CARD_VALUE, MAIN_MODE_MONSTER_QUANTITY, &totalCards);
    generateEncounter(cardPool, POTION, MIN_CARD_VALUE, MAIN_MODE_POTIONS_QUANTITY, &totalCards);
    generateEncounter(cardPool, WEAPON, MIN_CARD_VALUE, MAIN_MODE_WEAPONS_QUANTITY, &totalCards);

    return totalCards;
}

// Debug deck, dealt unshuffled. Monsters only add up to 14, so you can't lose.
int generateRiggedCardPool(Card* cardPool) {
    int totalCards = 0;

    generateEncounter(cardPool, WEAPON, RIGGED_WEAPON_STARTING_VALUE, RIGGED_WEAPONS_QUANTITY, &totalCards);
    generateEncounter(cardPool, MONSTER, MIN_CARD_VALUE, RIGGED_MONSTER_QUANTITY, &totalCards);
    generateEncounter(cardPool, POTION, MIN_CARD_VALUE, RIGGED_POTIONS_QUANTITY, &totalCards);

    return totalCards;
}

// Generates an encounter based off of passed values and puts total out to a variable if desired.
void generateEncounter(Card* cardPool, EncounterType type, int startingValue, int totalToCreate, int* runningTotal) {
                        //REMEMBER. TRUE CONDITION ? FALSE CONDITION : ELSE CONDITION
    int startingIndex = (runningTotal != NULL) ? *runningTotal : 0;

    if (startingIndex + totalToCreate > DECK_SIZE) return;

    if (startingValue < MIN_CARD_VALUE) {
        startingValue = MIN_CARD_VALUE;
    }

    for (int incrementer = 0; incrementer < totalToCreate; incrementer++) {
        int currentIndex = startingIndex + incrementer;

        if (startingValue > MAX_MONSTER_ATTACK_VALUE && type == MONSTER) {   // && means AND: both must be true
            startingValue = MIN_CARD_VALUE;
        }

        cardPool[currentIndex].id = currentIndex + 1;
        cardPool[currentIndex].type = type;
        cardPool[currentIndex].value = startingValue++;
    }

    if (runningTotal != NULL) {
        *runningTotal += totalToCreate;   // same as: *runningTotal = *runningTotal + totalToCreate
    }
}

void cardShuffle(Card** cardArray, int totalCards) {
    for (int currentSlot = totalCards - 1; currentSlot > 0; currentSlot--) {
        int randomSlot = rand() % (currentSlot + 1);                           // random slot from 0 up to currentSlot

        Card* cardInHand = cardArray[currentSlot];
        cardArray[currentSlot] = cardArray[randomSlot];
        cardArray[randomSlot] = cardInHand;
    }
}

// Deals the pool into the ring in order. Shuffle afterwards with cardShuffle.
void buildDeck(Game* game, int totalCards) {
    if (totalCards <= 0 || totalCards > DECK_SIZE) return;   // || means OR: no cards, or too many

    Zone* deck = &game->mainDeck;

    for (int currentSlot = 0; currentSlot < DECK_SIZE; currentSlot++) {
                                    //REMEMBER. TRUE CONDITION ? FALSE CONDITION : ELSE CONDITION
        deck->cards[currentSlot] = (currentSlot < totalCards) ? &game->globalCardPool[currentSlot] : NULL;
    }

    deck->topIndex = 0;
    deck->count = totalCards;
}

void setPlayerDefault(Player* playerOne) {
    playerOne->health = PLAYER_DEFAULT_HEALTH;
    playerOne->canFlee = PLAYER_DEFAULT_FLEE_STATE;
    playerOne->potionUsedThisTurn = PLAYER_DEFAULT_POTION_STATE;
    playerOne->weapon.equipped = PLAYER_DEFAULT_WEAPON;
    playerOne->weapon.killCount = PLAYER_DEFAULT_KILL_COUNT;

    for (int stackIndex = 0; stackIndex < MAX_MONSTER_WEAPON_STACK; stackIndex++) {
        playerOne->weapon.monsterStack[stackIndex] = NULL;
    }
}

// ========================================================
// Managers
// ========================================================
void dealRoomCards(Game* game) {
    Zone* deck = &game->mainDeck;

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        if (deck->count == 0) return;
        if (!isRoomSlotEmpty(game, roomSlot)) continue;   // If the slot is not empty, skip it

        game->roomSlots[roomSlot] = drawTopCard(deck);
    }
}

// Fleeing passes false so the player can't flee twice in a row.
void advanceToNextRoom(Game* game, bool canFleeNextRoom) {
    dealRoomCards(game);
    startNewTurn(&game->playerOne);
    setPlayerCanFlee(&game->playerOne, canFleeNextRoom);
}

void returnRoomToDeck(Game* game) {
    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        if (isRoomSlotEmpty(game, roomSlot)) continue;

        placeAtBottom(&game->mainDeck, game->roomSlots[roomSlot]);
        game->roomSlots[roomSlot] = NULL;
    }
}

FleeResult fleeManager(Game* game) {
    if (game->playerOne.canFlee == false) return FLEE_BLOCKED;

    returnRoomToDeck(game);
    advanceToNextRoom(game, false);

    return FLEE_RESOLVED;
}

EncounterResult encounterManager(Game* game, int chosenSlot, CombatChoice combatChoice) {
    if (!canEncounterCards(game)) return ENCOUNTER_BLOCKED_ROOM_NOT_CLEARED;   // If no cards can be faced, stop here
    if (isRoomSlotEmpty(game, chosenSlot)) return ENCOUNTER_BLOCKED_EMPTY_SLOT;

    Card* cardOnTable = game->roomSlots[chosenSlot];

    Player* player = &game->playerOne;
    Zone* discardPile = &game->discardPile;

    setPlayerCanFlee(player, false);

    game->roomSlots[chosenSlot] = NULL;
    game->lastResolvedCard = cardOnTable;

    switch (cardOnTable->type) {
        case MONSTER:
            combatManager(player, cardOnTable, discardPile, combatChoice);
            break;

        case POTION:
            healManager(player, cardOnTable, discardPile);
            break;

        case WEAPON:
            equipWeapon(player, cardOnTable, discardPile);
            break;

        default:
            placeAtBottom(discardPile, cardOnTable);
            break;
    }

    return ENCOUNTER_RESOLVED;
}

// Bare-handed kills are discarded. Weapon kills stack on the weapon.
void combatManager(Player* player, Card* monster, Zone* discardPile, CombatChoice combatChoice) {
    int damageTaken = decideDamageValue(player, monster, combatChoice);

    applyDamage(player, damageTaken);

    // If the player went bare-handed, or the weapon can't be used on this monster
    if (combatChoice == COMBAT_CHOICE_BARE_HANDED || !weaponUsableOnMonster(player, monster)) {
        placeAtBottom(discardPile, monster);
        return;
    }

    player->weapon.monsterStack[player->weapon.killCount] = monster;
    player->weapon.killCount++;
}

// A second potion in the same turn is still used up, it just doesn't heal.
void healManager(Player* player, Card* potion, Zone* discardPile) {
    if (!player->potionUsedThisTurn) {   // If no potion has been drunk yet this turn
        applyHeal(player, potion->value);
        player->potionUsedThisTurn = true;
    }

    placeAtBottom(discardPile, potion);
}

void equipWeapon(Player* player, Card* weapon, Zone* discardPile) {
    discardEquippedWeapon(player, discardPile);

    player->weapon.equipped = weapon;
}

void discardEquippedWeapon(Player* player, Zone* discardPile) {
    if (player->weapon.equipped == NULL) return;

    for (int stackIndex = 0; stackIndex < player->weapon.killCount; stackIndex++) {
        placeAtBottom(discardPile, player->weapon.monsterStack[stackIndex]);
        player->weapon.monsterStack[stackIndex] = NULL;
    }

    placeAtBottom(discardPile, player->weapon.equipped);

    player->weapon.equipped = PLAYER_DEFAULT_WEAPON;
    player->weapon.killCount = PLAYER_DEFAULT_KILL_COUNT;
}

// ========================================================
// Scoring
// ========================================================
int calculateFinalScore(Game* game) {
    Player* player = &game->playerOne;

    if (isPlayerDead(player)) {
        int remainingMonsterTotal = sumRemainingMonsterValues(&game->mainDeck);

        return -remainingMonsterTotal;   // the - makes it a negative score
    }

    if (hasPotionVictoryBonus(game)) {
        int potionBonus = game->lastResolvedCard->value;

        return player->health + potionBonus;
    }

    return player->health;
}

int sumRemainingMonsterValues(Zone* pile) {
    int runningTotal = 0;

    for (int position = 0; position < pile->count; position++) {
        Card* currentCard = cardAtPosition(pile, position);

        if (currentCard->type == MONSTER) {
            runningTotal += currentCard->value;   // same as: runningTotal = runningTotal + currentCard->value
        }
    }

    return runningTotal;
}

bool hasPotionVictoryBonus(Game* game) {
    if (game->lastResolvedCard == NULL) return false;
    if (game->lastResolvedCard->type != POTION) return false;
    if (game->playerOne.health != PLAYER_MAX_HEALTH) return false;

    return true;
}

// ========================================================
// Helpers: Questions
// ========================================================
bool isRoomSlotEmpty(Game* game, int slotIndex) {
    return (game->roomSlots[slotIndex] == NULL);
}

bool isPlayerDead(Player* player) {
    return (player->health <= PLAYER_MINIMUM_HEALTH);
}

bool isDungeonCleared(Game* game) {
    if (game->mainDeck.count > 0) return false;
    if (countCardsInRoom(game) > 0) return false;

    return true;
}

bool isGameOver(Game* game) {
    if (isPlayerDead(&game->playerOne)) return true;
    if (isDungeonCleared(game)) return true;

    return false;
}

bool canEncounterCards(Game* game) {
    if (countCardsInRoom(game) > 1) return true;
    if (game->mainDeck.count == 0) return true;

    return false;
}

bool isRoomComplete(Game* game) {
    if (isGameOver(game)) return false;
    if (canEncounterCards(game)) return false;

    return true;
}

bool isGameSessionActive(Game* game) {
    return (game != NULL);
}

// ========================================================
// Helpers: Combat
// ========================================================
// Can be negative when the weapon outclasses the monster.
int decideDamageValue(Player* player, Card* monster, CombatChoice combatChoice) {
    if (combatChoice != COMBAT_CHOICE_USE_WEAPON) return monster->value;
    if (!weaponUsableOnMonster(player, monster)) return monster->value;   // If the weapon can't be used on this monster

    int weaponValue = getEquippedWeaponValue(player);

    return monster->value - weaponValue;
}

bool weaponUsableOnMonster(Player* player, Card* monster) {
    if (player->weapon.equipped == NULL) return false;                 // no weapon
    if (player->weapon.killCount == 0) return true;                    // fresh weapon
    if (monster->value <= getLastKillValue(player)) return true;       // combo still going

    return false;
}

// ========================================================
// Helpers: Previews for the screens
// ========================================================
EncounterPrompt requiredEncounterPrompt(Game* game, int slotIndex) {
    if (isRoomSlotEmpty(game, slotIndex)) return ENCOUNTER_PROMPT_NONE;

    Player* player = &game->playerOne;
    Card* chosenCard = game->roomSlots[slotIndex];

    switch (chosenCard->type) {
        case MONSTER:
            if (weaponUsableOnMonster(player, chosenCard)) return ENCOUNTER_PROMPT_COMBAT_CHOICE;
            return ENCOUNTER_PROMPT_BARE_HANDED_CONFIRM;

        case WEAPON:
            if (pendingWeaponDiscardCount(player) > 0) return ENCOUNTER_PROMPT_WEAPON_SWAP;
            return ENCOUNTER_PROMPT_NONE;

        case POTION:
            if (wouldPotionBeWasted(player)) return ENCOUNTER_PROMPT_POTION_WASTE;
            return ENCOUNTER_PROMPT_NONE;

        default:
            return ENCOUNTER_PROMPT_NONE;
    }
}

int getSlotCardValue(Game* game, int slotIndex) {
    if (isRoomSlotEmpty(game, slotIndex)) return 0;

    return game->roomSlots[slotIndex]->value;
}

int previewDamageTaken(Game* game, int slotIndex, CombatChoice combatChoice) {
    if (isRoomSlotEmpty(game, slotIndex)) return 0;

    Card* chosenCard = game->roomSlots[slotIndex];
    int rawDamage = decideDamageValue(&game->playerOne, chosenCard, combatChoice);
    int damageTaken = preventNegative(rawDamage);

    return damageTaken;
}

int pendingWeaponDiscardCount(Player* player) {
    if (player->weapon.equipped == NULL) return 0;

    return player->weapon.killCount + 1; // + 1 for the weapon itself
}

bool wouldPotionBeWasted(Player* player) {
    return player->potionUsedThisTurn;
}

// ========================================================
// Helpers: Health
// ========================================================
// Negative amounts count as 0, so damage never heals and potions never hurt.
void applyDamage(Player* player, int damageTaken) {
    int damage = preventNegative(damageTaken);
    int newHealth = clamp(player->health - damage, PLAYER_MINIMUM_HEALTH, PLAYER_MAX_HEALTH);

    setPlayerHealth(player, newHealth);
}

void applyHeal(Player* player, int healValue) {
    int heal = preventNegative(healValue);
    int newHealth = clamp(player->health + heal, PLAYER_MINIMUM_HEALTH, PLAYER_MAX_HEALTH);

    setPlayerHealth(player, newHealth);
}

void setPlayerHealth(Player* player, int valueToSet) {
    player->health = valueToSet;
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
// Helpers: Player and room state
// ========================================================
void setPlayerCanFlee(Player* player, bool state) {
    if (player == NULL) return;

    player->canFlee = state;
}

void startNewTurn(Player* player) {
    player->potionUsedThisTurn = PLAYER_DEFAULT_POTION_STATE;
}

int countCardsInRoom(Game* game) {
    int cardsFound = 0;

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        if (!isRoomSlotEmpty(game, roomSlot)) {   // If the slot is not empty
            cardsFound++;
        }
    }

    return cardsFound;
}

int getEquippedWeaponValue(Player* player) {
    if (player->weapon.equipped == NULL) return 0;

    return player->weapon.equipped->value;
}

int getLastKillValue(Player* player) {
    if (player->weapon.killCount == 0) return 0;

    int lastKillIndex = player->weapon.killCount - 1;

    return player->weapon.monsterStack[lastKillIndex]->value;
}
