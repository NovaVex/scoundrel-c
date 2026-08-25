// ========================================================
// Game_mechanics.c
// --------------------------------------------------------
// JOB
//   The rules. This is the file that actually knows how
//   Scoundrel is played. It is the only file that changes
//   game state.
//
// THE HARD RULE
//   No stdio. No printf. Not one. If this file needs to tell
//   the player something, it returns a value and lets the
//   layers above decide how to say it. That is what keeps it
//   portable into a game engine later.
//
// WHO CALLS THIS FILE
//   Game_master.c    to run turns
//   Scene_manager.c  to ask what is legal before asking the player
//   UI_manager.c     to ask for numbers to display (read only)
//
// MAP OF THIS FILE, IN ORDER
//   CARD MANIPULATION  moving cards between piles
//   BUILDERS           making the deck and the starting player
//   MANAGERS           the ones that resolve a turn:
//                      encounterManager, combatManager,
//                      healManager, equipWeapon, fleeManager
//   SCORING            final score at game over
//   HELPERS            the questions. Most are one line and
//                      answer exactly one thing. This is the
//                      long section, but nothing in it is deep.
// ========================================================

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

void placeAtBottom(Zone* pile, CardLink* card) {
    if (card == NULL) return;

    card->next = NULL;

    if (pile->count == 0) {
        pile->topCard = card;
        pile->bottomCard = card;
        pile->count++;
        return;
    }

    pile->bottomCard->next = card;
    pile->bottomCard = card;
    pile->count++;
}

// ========================================================
// Builders
// ========================================================
void generateCardPool(Game* game, int* outTotalCards) {
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

void buildDeck(Game* game, int totalCards) {
    CardLink* shuffleArray[DECK_SIZE];

    if (totalCards <= 0 || totalCards > DECK_SIZE) return;

    for (int currentSlot = 0; currentSlot < totalCards; currentSlot++) {
        CardLink* currentLink = &game->nodePool[currentSlot];

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

    game->mainDeck.bottomCard = shuffleArray[lastCardIndex];
    game->mainDeck.count = totalCards;
}

void setPlayerDefault(Player* playerOne) {
    playerOne->minHealth = MINIMUM_HEALTH;
    playerOne->maxHealth = STARTING_HEALTH;
    playerOne->health = STARTING_HEALTH;
    playerOne->canFlee = true;
    playerOne->potionUsedThisTurn = false;

    playerOne->weapon.equipped = NULL;
    playerOne->weapon.killCount = 0;

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

        if (isRoomSlotEmpty(game, roomSlot)) {
            game->roomSlots[roomSlot] = drawTopCard(deck);
        }
    }
}

void advanceToNextRoom(Game* game) {
    dealRoomCards(game);
    startNewTurn(&game->playerOne);
    setCanFleeTrue(&game->playerOne);
}

FleeResult fleeManager(Game* game) {
    if (game->playerOne.canFlee == false) return FLEE_BLOCKED;

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        if (isRoomSlotEmpty(game, roomSlot)) continue;

        placeAtBottom(&game->mainDeck, game->roomSlots[roomSlot]);
        game->roomSlots[roomSlot] = NULL;
    }

    dealRoomCards(game);
    startNewTurn(&game->playerOne);
    setCanFleeFalse(&game->playerOne);

    return FLEE_RESOLVED;
}

EncounterResult encounterManager(Game* game, int chosenSlot, CombatChoice combatChoice) {
    if (!canEncounterCards(game)) return ENCOUNTER_BLOCKED_ROOM_NOT_CLEARED;
    if (isRoomSlotEmpty(game, chosenSlot)) return ENCOUNTER_BLOCKED_EMPTY_SLOT;

    CardLink* cardLinkOnTable = game->roomSlots[chosenSlot];
    Card* actualCard = cardLinkOnTable->data;

    Player* player = &game->playerOne;
    Zone* discardPile = &game->discardPile;

    setCanFleeFalse(player);

    game->roomSlots[chosenSlot] = NULL;
    game->lastResolvedCard = actualCard;

    switch (actualCard->type) {
        case MONSTER:
            combatManager(player, cardLinkOnTable, discardPile, combatChoice);
            break;

        case POTION:
            healManager(player, cardLinkOnTable, discardPile);
            break;

        case WEAPON:
            equipWeapon(player, cardLinkOnTable, discardPile);
            break;

        default:
            placeAtBottom(discardPile, cardLinkOnTable);
            break;
    }

    return ENCOUNTER_RESOLVED;
}

void combatManager(Player* player, CardLink* monsterLink, Zone* discardPile, CombatChoice combatChoice) {
    Card* monster = monsterLink->data;
    bool useWeapon = willUseWeapon(player, monster, combatChoice);

    int damageTaken = decideDamageValue(player, monster, combatChoice);
    int newHealth = clampedDamageToPlayer(player->health, player->minHealth, player->maxHealth, damageTaken);

    setPlayerHealth(player, newHealth);

    if (!useWeapon) {
        placeAtBottom(discardPile, monsterLink);
        return;
    }

    if (player->weapon.killCount >= MAX_MONSTER_WEAPON_STACK) {
        placeAtBottom(discardPile, monsterLink);
        return;
    }

    player->weapon.monsterStack[player->weapon.killCount] = monsterLink;
    player->weapon.killCount++;
}

void healManager(Player* player, CardLink* potionLink, Zone* discardPile) {
    if (player->potionUsedThisTurn) {
        placeAtBottom(discardPile, potionLink);
        return;
    }

    int healValue = potionLink->data->value;
    int newHealth = clampedPlayerHeal(player->health, player->minHealth, player->maxHealth, healValue);

    setPlayerHealth(player, newHealth);
    player->potionUsedThisTurn = true;

    placeAtBottom(discardPile, potionLink);
}

void equipWeapon(Player* player, CardLink* weaponLink, Zone* discardPile) {
    discardEquippedWeapon(player, discardPile);

    player->weapon.equipped = weaponLink;
    player->weapon.killCount = 0;
}

void discardEquippedWeapon(Player* player, Zone* discardPile) {
    if (player->weapon.equipped == NULL) return;

    for (int stackIndex = 0; stackIndex < player->weapon.killCount; stackIndex++) {
        placeAtBottom(discardPile, player->weapon.monsterStack[stackIndex]);
        player->weapon.monsterStack[stackIndex] = NULL;
    }

    placeAtBottom(discardPile, player->weapon.equipped);

    player->weapon.equipped = NULL;
    player->weapon.killCount = 0;
}

// ========================================================
// Scoring
// ========================================================
int calculateFinalScore(Game* game) {
    Player* player = &game->playerOne;

    if (isPlayerDead(player)) {
        return -sumRemainingMonsterValues(&game->mainDeck);
    }

    if (hasPotionVictoryBonus(game)) {
        return player->health + game->lastResolvedCard->value;
    }

    return player->health;
}

int sumRemainingMonsterValues(Zone* pile) {
    int runningTotal = 0;
    CardLink* currentLink = pile->topCard;

    while (currentLink != NULL) {
        if (currentLink->data->type == MONSTER) {
            runningTotal += currentLink->data->value;
        }
        currentLink = currentLink->next;
    }

    return runningTotal;
}

bool hasPotionVictoryBonus(Game* game) {
    if (game->lastResolvedCard == NULL) return false;
    if (game->lastResolvedCard->type != POTION) return false;
    if (game->playerOne.health != game->playerOne.maxHealth) return false;

    return true;
}

// ========================================================
// Helpers
// ========================================================
bool isRoomSlotEmpty(Game* game, int slotIndex) {
    return (game->roomSlots[slotIndex] == NULL);
}

bool isPlayerDead(Player* player) {
    return (player->health <= 0);
}

bool isDungeonCleared(Game* game) {
    if (game->mainDeck.count > 0) return false;
    if (countCardsInRoom(game) > 0) return false;

    return true;
}

bool isGameOver(Game* game) {
    if (isPlayerDead(&game->playerOne)) return true;

    return isDungeonCleared(game);
}

bool canEncounterCards(Game* game) {
    if (countCardsInRoom(game) > 1) return true;
    if (game->mainDeck.count == 0) return true;

    return false;
}

bool isRoomComplete(Game* game) {
    if (isGameOver(game)) return false;

    return !canEncounterCards(game);
}

bool isGameSessionActive(Game* game) {
    return (game != NULL);
}

int decideDamageValue(Player* player, Card* monster, CombatChoice combatChoice) {
    WeaponState currentState = checkWeaponState(player, monster);

    switch (currentState) {
        case WEAPON_FRESH:
        case WEAPON_VALID_COMBO:
            if (combatChoice == COMBAT_CHOICE_BARE_HANDED) return monster->value;
            return monster->value - getEquippedWeaponValue(player);

        case WEAPON_NONE:
        case WEAPON_INVALID_COMBO:
        default:
            return monster->value;
    }
}

WeaponState checkWeaponState(Player* player, Card* monster) {
    if (player->weapon.equipped == NULL) return WEAPON_NONE;
    if (player->weapon.killCount == 0) return WEAPON_FRESH;

    if (monster->value <= getLastKillValue(player)) return WEAPON_VALID_COMBO;

    return WEAPON_INVALID_COMBO;
}

bool willUseWeapon(Player* player, Card* monster, CombatChoice combatChoice) {
    if (combatChoice != COMBAT_CHOICE_USE_WEAPON) return false;

    return weaponUsableOnMonster(player, monster);
}

bool weaponUsableOnMonster(Player* player, Card* monster) {
    WeaponState currentState = checkWeaponState(player, monster);

    switch (currentState) {
        case WEAPON_FRESH:
        case WEAPON_VALID_COMBO:
            return true;

        case WEAPON_NONE:
        case WEAPON_INVALID_COMBO:
        default:
            return false;
    }
}

EncounterPrompt requiredEncounterPrompt(Game* game, int slotIndex) {
    if (isRoomSlotEmpty(game, slotIndex)) return ENCOUNTER_PROMPT_NONE;

    Player* player = &game->playerOne;
    Card* chosenCard = game->roomSlots[slotIndex]->data;

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

    return game->roomSlots[slotIndex]->data->value;
}

int previewDamageTaken(Game* game, int slotIndex, CombatChoice combatChoice) {
    if (isRoomSlotEmpty(game, slotIndex)) return 0;

    Card* chosenCard = game->roomSlots[slotIndex]->data;
    int rawDamage = decideDamageValue(&game->playerOne, chosenCard, combatChoice);

    return preventNegative(rawDamage);
}

int pendingWeaponDiscardCount(Player* player) {
    if (player->weapon.equipped == NULL) return 0;

    return player->weapon.killCount + 1;
}

bool wouldPotionBeWasted(Player* player) {
    return player->potionUsedThisTurn;
}

int clampedDamageToPlayer(int rawHealth, int minHealth, int maxHealth, int rawDamageDealt) {
    rawDamageDealt = preventNegative(rawDamageDealt);
    int newHealth = damageCalculation(rawHealth, rawDamageDealt);

    return clamp(newHealth, minHealth, maxHealth);
}

int damageCalculation(int currentHealth, int damageTaken) {
    return currentHealth - damageTaken;
}

int healCalculation(int currentHealth, int healValue) {
    return currentHealth + healValue;
}

int clampedPlayerHeal(int rawHealth, int minHealth, int maxHealth, int rawHeal) {
    rawHeal = preventNegative(rawHeal);
    int newHealth = healCalculation(rawHealth, rawHeal);

    return clamp(newHealth, minHealth, maxHealth);
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

void setCanFleeFalse(Player* player) {
    player->canFlee = false;
}

void setCanFleeTrue(Player* player) {
    player->canFlee = true;
}

void startNewTurn(Player* player) {
    player->potionUsedThisTurn = false;
}

int countCardsInRoom(Game* game) {
    int cardsFound = 0;

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        if (!isRoomSlotEmpty(game, roomSlot)) cardsFound++;
    }

    return cardsFound;
}

int getEquippedWeaponValue(Player* player) {
    if (player->weapon.equipped == NULL) return 0;

    return player->weapon.equipped->data->value;
}

int getLastKillValue(Player* player) {
    if (player->weapon.killCount == 0) return 0;

    int lastKillIndex = player->weapon.killCount - 1;

    return player->weapon.monsterStack[lastKillIndex]->data->value;
}
