#pragma once

void generateDeck(struct deck* generate);
void shuffleDeck(struct deck* deck);
void dealRoomCards(struct game* game);
void encounterManager(struct game* game, int encounterSlot);
void combatManager(struct game* game, int encounterSlot);
void healManager(struct game* game, int encounterSlot);
void equipWeapon(struct game* game, int encounterSlot);
void copyCard(struct card* source, struct card* targetArray, int* targetCounter);
void emptyCardSlot(struct card* slotToClear);
int checkWeaponState(struct player* player);
int healCalculation(int currentHp, int healValue);
int clampedPlayerHeal(int rawHp, int minHp, int maxHp, int rawHeal);
int clampedDamageToPlayer(int rawHp, int minHp, int maxHp, int rawDamageDealt);
void setPlayerHealth(struct player* player, int valueToSet);
int clamp(int value, int min, int max);
int preventNegative(int value);
int damageCalculation(int currentHP, int damageTaken);

