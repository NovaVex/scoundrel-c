#pragma once
#include <stdbool.h>
#include <iso646.h>

//Deck Definitions
#define DECK_SIZE 52
#define MIN_CARD_VALUE 2
#define MAX_MONSTER_ATTACK_VALUE 14
#define MAIN_MODE_MONSTER_QUANTITY 26
#define MAIN_MODE_POTIONS_QUANTITY 9
#define MAIN_MODE_WEAPONS_QUANTITY 9

//Rigged Deck Definitions
#define RIGGED_WEAPON_STARTING_VALUE 5
#define RIGGED_WEAPONS_QUANTITY 2
#define RIGGED_MONSTER_QUANTITY 4
#define RIGGED_POTIONS_QUANTITY 2

//Room Definitions
#define MAX_ROOM_SIZE 4
#define MAX_MONSTER_WEAPON_STACK 26

//Player Definitions
#define PLAYER_DEFAULT_HEALTH 20
#define PLAYER_DEFAULT_FLEE_STATE true
#define PLAYER_DEFAULT_POTION_STATE false
#define PLAYER_DEFAULT_WEAPON NULL
#define PLAYER_DEFAULT_KILL_COUNT 0
#define PLAYER_MINIMUM_HEALTH 0
#define PLAYER_MAX_HEALTH 20

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

// A fixed ring of card pointers. topIndex is the top card; drawing
// and discarding move topIndex and count, never the cards.
typedef struct Zone {
    Card* cards[DECK_SIZE];
    int topIndex;
    int count;
} Zone;

// ========================================================
// The player
// ========================================================
typedef struct Weapon {
    Card* equipped;
    Card* monsterStack[MAX_MONSTER_WEAPON_STACK];
    int killCount;
} Weapon;

typedef struct Player {
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
    Zone mainDeck;
    Zone discardPile;
    Card* roomSlots[MAX_ROOM_SIZE];
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
Card* drawTopCard(Zone* pile);
void placeAtBottom(Zone* pile, Card* card);
Card* cardAtPosition(Zone* pile, int position);

// ========================================================
// Builders
// ========================================================
int generateGlobalCardPool(Card* cardPool);
int generateRiggedCardPool(Card* cardPool);
void generateEncounter(Card* cardPool, EncounterType type, int startingValue, int totalToCreate, int* runningTotal);
void cardShuffle(Card** cardArray, int totalCards);
void buildDeck(Game* game, int totalCards);
void setPlayerDefault(Player* playerOne);

// ========================================================
// Managers
// ========================================================
void dealRoomCards(Game* game);
void advanceToNextRoom(Game* game, bool canFleeNextRoom);
void returnRoomToDeck(Game* game);
FleeResult fleeManager(Game* game);
EncounterResult encounterManager(Game* game, int chosenSlot, CombatChoice combatChoice);

void combatManager(Player* player, Card* monster, Zone* discardPile, CombatChoice combatChoice);
void healManager(Player* player, Card* potion, Zone* discardPile);
void equipWeapon(Player* player, Card* weapon, Zone* discardPile);
void discardEquippedWeapon(Player* player, Zone* discardPile);

// ========================================================
// Scoring
// ========================================================
int calculateFinalScore(Game* game);
int sumRemainingMonsterValues(Zone* pile);
bool hasPotionVictoryBonus(Game* game);

// ========================================================
// Helpers: Questions
// ========================================================
bool isRoomSlotEmpty(Game* game, int slotIndex);
bool isPlayerDead(Player* player);
bool isDungeonCleared(Game* game);
bool isGameOver(Game* game);
bool canEncounterCards(Game* game);
bool isRoomComplete(Game* game);
bool isGameSessionActive(Game* game);

// ========================================================
// Helpers: Combat
// ========================================================
int decideDamageValue(Player* player, Card* monster, CombatChoice combatChoice);
bool weaponUsableOnMonster(Player* player, Card* monster);

// ========================================================
// Helpers: Previews for the screens
// ========================================================
EncounterPrompt requiredEncounterPrompt(Game* game, int slotIndex);
int getSlotCardValue(Game* game, int slotIndex);
int previewDamageTaken(Game* game, int slotIndex, CombatChoice combatChoice);
int pendingWeaponDiscardCount(Player* player);
bool wouldPotionBeWasted(Player* player);

// ========================================================
// Helpers: Health
// ========================================================
void applyDamage(Player* player, int damageTaken);
void applyHeal(Player* player, int healValue);
void setPlayerHealth(Player* player, int valueToSet);
int clamp(int value, int minimum, int maximum);
int preventNegative(int value);

// ========================================================
// Helpers: Player and room state
// ========================================================
void setPlayerCanFlee(Player* player, bool state);
void startNewTurn(Player* player);
int countCardsInRoom(Game* game);
int getEquippedWeaponValue(Player* player);
int getLastKillValue(Player* player);
