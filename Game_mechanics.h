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
void generateCardPool(Game* game, int* outTotalCards);
void cardShuffle(CardLink** cardArray, int totalCards);
void buildDeck(Game* game, int totalCards);
void setPlayerDefault(Player* playerOne);

// ========================================================
// Managers
// ========================================================
void dealRoomCards(Game* game);
void advanceToNextRoom(Game* game);
FleeResult fleeManager(Game* game);
EncounterResult encounterManager(Game* game, int chosenSlot, CombatChoice combatChoice);
void combatManager(Player* player, CardLink* monsterLink, Zone* discardPile, CombatChoice combatChoice);
void healManager(Player* player, CardLink* potionLink, Zone* discardPile);
void equipWeapon(Player* player, CardLink* weaponLink, Zone* discardPile);
void discardEquippedWeapon(Player* player, Zone* discardPile);

// ========================================================
// Scoring
// ========================================================
int calculateFinalScore(Game* game);
int sumRemainingMonsterValues(Zone* pile);
bool hasPotionVictoryBonus(Game* game);

// ========================================================
// Helpers
// ========================================================
bool isRoomSlotEmpty(Game* game, int slotIndex);
bool isPlayerDead(Player* player);
bool isDungeonCleared(Game* game);
bool isGameOver(Game* game);
bool canEncounterCards(Game* game);
bool isRoomComplete(Game* game);
bool isGameSessionActive(Game* game);
int decideDamageValue(Player* player, Card* monster, CombatChoice combatChoice);
WeaponState checkWeaponState(Player* player, Card* monster);
bool weaponUsableOnMonster(Player* player, Card* monster);
bool willUseWeapon(Player* player, Card* monster, CombatChoice combatChoice);
EncounterPrompt requiredEncounterPrompt(Game* game, int slotIndex);
int getSlotCardValue(Game* game, int slotIndex);
int previewDamageTaken(Game* game, int slotIndex, CombatChoice combatChoice);
int pendingWeaponDiscardCount(Player* player);
bool wouldPotionBeWasted(Player* player);
int clampedDamageToPlayer(int rawHealth, int minHealth, int maxHealth, int rawDamageDealt);
int damageCalculation(int currentHealth, int damageTaken);
int healCalculation(int currentHealth, int healValue);
int clampedPlayerHeal(int rawHealth, int minHealth, int maxHealth, int rawHeal);
void setPlayerHealth(Player* player, int valueToSet);
int clamp(int value, int minimum, int maximum);
int preventNegative(int value);
void setCanFleeFalse(Player* player);
void setCanFleeTrue(Player* player);
void startNewTurn(Player* player);
int countCardsInRoom(Game* game);
int getEquippedWeaponValue(Player* player);
int getLastKillValue(Player* player);
