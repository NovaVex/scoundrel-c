#pragma once
#include <stdbool.h>

#define DECK_SIZE 52
#define MAX_ROOM_SIZE 4
#define MAX_MONSTER_WEAPON_STACK 26
#define STARTING_HEALTH 20
#define MINIMUM_HEALTH 0

// ========================================================
// Cards and piles
// ========================================================
typedef enum EncounterType {
    EMPTY = 'E',
    MONSTER = 'M',
    POTION = 'P',
    WEAPON = 'W',
    FLEE = 'F'
} EncounterType;

typedef struct Card {
    int id;
    char type;
    int value;
} Card;

typedef struct CardLink {
    Card* data;
    struct CardLink* next;
} CardLink;

typedef struct Zone {
    CardLink* topCard;
    CardLink* bottomCard;
    int count;
} Zone;

// ========================================================
// The player
// ========================================================
typedef struct Weapon {
    CardLink* equipped;
    CardLink* monsterStack[MAX_MONSTER_WEAPON_STACK];
    int killCount;
} Weapon;

typedef struct Player {
    int minHealth;
    int maxHealth;
    int health;
    Weapon weapon;
    bool canFlee;
    bool potionUsedThisTurn;
} Player;

// ========================================================
// The session
// ========================================================
typedef struct Game {
    Card globalCardPool[DECK_SIZE];
    CardLink nodePool[DECK_SIZE];
    Zone mainDeck;
    Zone discardPile;
    CardLink* roomSlots[MAX_ROOM_SIZE];
    Player playerOne;
    Card* lastResolvedCard;
} Game;

// ========================================================
// Answers the rules hand back
// ========================================================
typedef enum CombatChoice {
    COMBAT_CHOICE_USE_WEAPON = 1,
    COMBAT_CHOICE_BARE_HANDED = 2
} CombatChoice;

typedef enum EncounterResult {
    ENCOUNTER_RESOLVED,
    ENCOUNTER_CANCELLED,
    ENCOUNTER_BLOCKED_EMPTY_SLOT,
    ENCOUNTER_BLOCKED_ROOM_NOT_CLEARED
} EncounterResult;

typedef enum EncounterPrompt {
    ENCOUNTER_PROMPT_NONE,
    ENCOUNTER_PROMPT_COMBAT_CHOICE,
    ENCOUNTER_PROMPT_BARE_HANDED_CONFIRM,
    ENCOUNTER_PROMPT_WEAPON_SWAP,
    ENCOUNTER_PROMPT_POTION_WASTE
} EncounterPrompt;

typedef enum FleeResult {
    FLEE_RESOLVED,
    FLEE_BLOCKED
} FleeResult;

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
