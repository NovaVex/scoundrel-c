// ========================================================
// Save.h
// --------------------------------------------------------
// Blueprint for the save system. Holds a session as plain
// card IDs, kept separate from the live game state.
//
// Save data, Translation helpers (Lookup, Packing, Unpacking),
// Save helpers, Load helpers, then the Core managers.
// ========================================================

#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "Game_mechanics.h"

//Save Definitions
#define SAVE_NO_CARD_ID 0    // an empty slot: no card is saved here
#define SAVE_LABEL_SIZE 32   // 31 characters, plus 1 for the \0 that ends the text

//Save Label Definitions
#define SAVE_LABEL_PLAYER_HEALTH "PlayerHealth:"
#define SAVE_LABEL_CAN_FLEE "CanFlee:"
#define SAVE_LABEL_POTION_USED "PotionUsed:"
#define SAVE_LABEL_WEAPON_ID "WeaponID:"
#define SAVE_LABEL_WEAPON_KILL_COUNT "WeaponKillCount:"
#define SAVE_LABEL_MONSTER_STACK "MonsterStack:"
#define SAVE_LABEL_LAST_RESOLVED "LastResolvedCardID:"
#define SAVE_LABEL_ROOM_SLOTS "RoomSlots:"

//Pile Label Definitions
#define SAVE_LABEL_MAIN_DECK_COUNT "MainDeck_Count:"
#define SAVE_LABEL_MAIN_DECK_TOP_INDEX "MainDeck_TopIndex:"
#define SAVE_LABEL_MAIN_DECK_IDS "MainDeck_IDs:"
#define SAVE_LABEL_DISCARD_COUNT "DiscardPile_Count:"
#define SAVE_LABEL_DISCARD_TOP_INDEX "DiscardPile_TopIndex:"
#define SAVE_LABEL_DISCARD_IDS "DiscardPile_IDs:"

// ========================================================
// Save Data Structures
// ========================================================
typedef struct PlayerSaveData {
    int playerHealth;
    bool canFlee;
    bool potionUsedThisTurn;
} PlayerSaveData;

typedef struct WeaponSaveData {
    int equippedWeaponId;
    int killedMonsterStackIds[MAX_MONSTER_WEAPON_STACK];
    int weaponKillCount;
} WeaponSaveData;

typedef struct PilesSaveData {
    int pileIds[DECK_SIZE];
    int pileTopIndex;
    int pileCount;
} PilesSaveData;

typedef struct GameSaveData {
    PlayerSaveData player;
    WeaponSaveData weapon;
    PilesSaveData mainDeck;
    PilesSaveData discardPile;
    int roomSlotIds[MAX_ROOM_SIZE];
    int lastResolvedCardId;
} GameSaveData;

// ========================================================
// Translation Helpers: ID Lookup
// ========================================================
Card* getCardById(Game* game, int targetId);
int getCardId(Card* card);

// ========================================================
// Translation Helpers: Packing (Live Game -> Save Data)
// ========================================================
void packPlayer(Player* livePlayer, PlayerSaveData* savedPlayer);
void packWeapon(Player* livePlayer, WeaponSaveData* savedWeapon);
void packPile(Zone* livePile, PilesSaveData* savedPile);
void packRoomSlots(Card** liveRoomSlots, int* savedRoomSlotIds);
void packGameSaveData(Game* liveGame, GameSaveData* saveData);

// ========================================================
// Translation Helpers: Unpacking (Save Data -> Live Game)
// ========================================================
void unpackPlayer(PlayerSaveData* savedPlayer, Player* livePlayer);
void unpackWeapon(Game* liveGame, WeaponSaveData* savedWeapon, Player* livePlayer);
void unpackPile(Game* liveGame, PilesSaveData* savedPile, Zone* livePile);
void unpackRoomSlots(Game* liveGame, int* savedRoomSlotIds, Card** liveRoomSlots);
void unpackGameSaveData(GameSaveData* saveData, Game* liveGame);

// ========================================================
// File Format Helpers
// ========================================================
bool readLabel(FILE* file, const char* expectedLabel);
void writeLabelledInt(FILE* file, const char* label, int value);
bool readLabelledInt(FILE* file, const char* label, int* valueOut);
void writeLabelledList(FILE* file, const char* label, const int* values, int total);
bool readLabelledList(FILE* file, const char* label, int* values, int total);

// ========================================================
// Save Helpers
// ========================================================
void savePlayer(FILE* file, PlayerSaveData* player);
void saveWeapon(FILE* file, WeaponSaveData* weapon);
void savePile(FILE* file, const char* countLabel, const char* topIndexLabel, const char* idsLabel, PilesSaveData* pile);

// ========================================================
// Load Helpers
// ========================================================
bool isPileSizeValid(PilesSaveData* pile);
bool loadPlayer(FILE* file, PlayerSaveData* player);
bool loadWeapon(FILE* file, WeaponSaveData* weapon);
bool loadPile(FILE* file, const char* countLabel, const char* topIndexLabel, const char* idsLabel, PilesSaveData* pile);

// ========================================================
// Core Save & Load Managers
// ========================================================
// Both say false if the file wouldn't open or wouldn't read. On false,
// the GameSaveData is part filled with junk, so don't unpack it.
bool saveGameData(const char* filepath, GameSaveData* data);
bool readGameSaveData(FILE* file, GameSaveData* data);
bool loadGameData(const char* filepath, GameSaveData* data);
