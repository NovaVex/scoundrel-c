#pragma once
#include <stdbool.h>
#include "Data_Structure.h"

// ========================================================
// Card Manipulation 
// ========================================================
CardLink* drawTopCard(Zone* pile);
void placeAtBottom(Zone* pile, CardLink* card);

// ========================================================
// Builders
// ========================================================
void generateCardPool(struct game* game, int* outTotalCards);
void cardShuffle(CardLink** cardArray, int totalCards);
void buildDeck(struct game* game, int totalCards);

// ========================================================
// Managers
// ========================================================
void dealRoomCards(struct game* game);
void fleeManager(struct game* game);
void encounterManager(struct game* game, int chosenSlot);
void combatManager(struct player* player, struct card* monster);
void healManager(struct player* player, struct card* potion);
void equipWeapon(struct player* player, struct card* encounterWeapon);

// ========================================================
// Helpers
// ========================================================
bool isRoomSlotEmpty(struct game* game, int slotIndex);
bool isPlayerDead(struct player* player);
int decideDamageValue(struct player* player, struct card* monster);
WeaponState checkWeaponState(struct player* player, struct card* monster);
int clampedDamageToPlayer(int rawHp, int minHp, int maxHp, int rawDamageDealt);
int damageCalculation(int currentHP, int damageTaken);
int healCalculation(int currentHp, int healValue);
int clampedPlayerHeal(int rawHp, int minHp, int maxHp, int rawHeal);
void setPlayerHealth(struct player* player, int valueToSet);
int clamp(int value, int min, int max);
int preventNegative(int value);
void setCanFleeFalse(struct player* player);
void setCanFleeTrue(struct player* player);
int countCardsInRoom(struct game* game);