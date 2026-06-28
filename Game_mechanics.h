#pragma once
#include <stdbool.h>
#include "Data_Structure.h"

// ========================================================
// Card Manipulation
// ========================================================
CardLink* drawTopCard(Zone* pile);
void placeAtBottom(Zone* pile, CardLink* cardToPlace);

// ========================================================
// Builders
// ========================================================
void generateCardPool(struct game* session, int* outTotalCards);
void cardShuffle(CardLink** cardArray, int totalCards);
void buildDeck(struct game* session, int totalCards);

// ========================================================
// Managers
// ========================================================
void dealRoomCards(struct game* session);
bool fleeManager(struct game* session);
EncounterResult encounterManager(struct game* session, int chosenSlot, bool fightBarehanded);
void combatManager(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded);
EncounterResult healManager(struct player* activePlayer, struct card* potionCard);
void equipWeapon(struct player* activePlayer, struct card* weaponCard);

// ========================================================
// Combat helpers
// ========================================================
void applyMonsterDamage(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded);
void stackMonsterOnWeapon(struct player* activePlayer, struct card* monsterCard);
bool weaponWasUsed(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded);
bool weaponIsUsableAgainst(struct player* activePlayer, struct card* monsterCard);
int decideDamageValue(struct player* activePlayer, struct card* monsterCard, bool fightBarehanded);
WeaponState checkWeaponState(struct player* activePlayer, struct card* monsterCard);

// ========================================================
// Rule queries
// ========================================================
bool isRoomSlotEmpty(struct game* session, int slotIndex);
bool isPlayerDead(struct player* activePlayer);
bool isDungeonCleared(struct game* session);
bool mustKeepLastCardForNextRoom(struct game* session);
bool roomIsReadyToLeave(struct game* session);
int countCardsInRoom(struct game* session);
int calculateFinalScore(struct game* session);

// ========================================================
// Health math
// ========================================================
int clampedDamageToPlayer(int rawHealth, int minimumHealth, int maximumHealth, int rawDamage);
int clampedPlayerHeal(int rawHealth, int minimumHealth, int maximumHealth, int rawHeal);
int damageCalculation(int currentHealth, int damageTaken);
int healCalculation(int currentHealth, int healValue);
void setPlayerHealth(struct player* activePlayer, int valueToSet);
int clamp(int value, int minimum, int maximum);
int preventNegative(int value);

// ========================================================
// Flag setters
// ========================================================
void setCanFleeFalse(struct player* activePlayer);
void setCanFleeTrue(struct player* activePlayer);
