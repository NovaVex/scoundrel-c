// ========================================================
// Save.c
// --------------------------------------------------------
// Reads, writes and translates a session. Packing turns live
// card pointers into IDs; unpacking turns the IDs back into
// pointers into the game's own card pool.
// ========================================================

#include "Save.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// ========================================================
// Translation Helpers: ID Lookup
// ========================================================

// Finds the live card carrying this ID, or NULL if there isn't one.
Card* getCardById(Game* game, int targetId) {
    if (targetId == SAVE_NO_CARD_ID) return NULL;

    for (int poolIndex = 0; poolIndex < DECK_SIZE; poolIndex++) {
        if (game->globalCardPool[poolIndex].id == targetId) {
            return &game->globalCardPool[poolIndex];
        }
    }

    return NULL;
}

// The other way round: a card's ID, or SAVE_NO_CARD_ID when the slot is empty.
int getCardId(Card* card) {
    if (card == NULL) return SAVE_NO_CARD_ID;

    return card->id;
}

// ========================================================
// Translation Helpers: Packing (Live Game -> Save Data)
// ========================================================
void packPlayer(Player* livePlayer, PlayerSaveData* savedPlayer) {
    if (livePlayer == NULL || savedPlayer == NULL) return;   // || means OR: either side missing, nothing to pack

    savedPlayer->playerHealth = livePlayer->health;
    savedPlayer->canFlee = livePlayer->canFlee;
    savedPlayer->potionUsedThisTurn = livePlayer->potionUsedThisTurn;
}

void packWeapon(Player* livePlayer, WeaponSaveData* savedWeapon) {
    if (livePlayer == NULL || savedWeapon == NULL) return;   // || means OR: no live player, or no slot to pack into

    savedWeapon->equippedWeaponId = getCardId(livePlayer->weapon.equipped);
    savedWeapon->weaponKillCount = livePlayer->weapon.killCount;

    for (int stackIndex = 0; stackIndex < MAX_MONSTER_WEAPON_STACK; stackIndex++) {
        Card* monster = livePlayer->weapon.monsterStack[stackIndex];

        savedWeapon->killedMonsterStackIds[stackIndex] = getCardId(monster);
    }
}

void packPile(Zone* livePile, PilesSaveData* savedPile) {
    if (livePile == NULL || savedPile == NULL) return;   // || means OR: no live pile, or no slot to pack into

    savedPile->pileCount = livePile->count;
    savedPile->pileTopIndex = 0;   // packing walks the ring top first, so a save always starts at slot 0

    for (int position = 0; position < livePile->count; position++) {
        Card* card = cardAtPosition(livePile, position);

        savedPile->pileIds[position] = getCardId(card);
    }
}

void packRoomSlots(Card** liveRoomSlots, int* savedRoomSlotIds) {
    if (liveRoomSlots == NULL || savedRoomSlotIds == NULL) return;   // || means OR: no live room, or no slot to pack into

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        savedRoomSlotIds[roomSlot] = getCardId(liveRoomSlots[roomSlot]);
    }
}

void packGameSaveData(Game* liveGame, GameSaveData* saveData) {
    if (liveGame == NULL || saveData == NULL) return;   // || means OR: no live game, or nowhere to save it

    packPlayer(&liveGame->playerOne, &saveData->player);
    packWeapon(&liveGame->playerOne, &saveData->weapon);
    packPile(&liveGame->mainDeck, &saveData->mainDeck);
    packPile(&liveGame->discardPile, &saveData->discardPile);
    packRoomSlots(liveGame->roomSlots, saveData->roomSlotIds);

    saveData->lastResolvedCardId = getCardId(liveGame->lastResolvedCard);
}

// ========================================================
// Translation Helpers: Unpacking (Save Data -> Live Game)
// ========================================================
void unpackPlayer(PlayerSaveData* savedPlayer, Player* livePlayer) {
    if (savedPlayer == NULL || livePlayer == NULL) return;   // || means OR: nothing saved, or no live player to fill

    livePlayer->health = savedPlayer->playerHealth;
    livePlayer->canFlee = savedPlayer->canFlee;
    livePlayer->potionUsedThisTurn = savedPlayer->potionUsedThisTurn;
}

void unpackWeapon(Game* liveGame, WeaponSaveData* savedWeapon, Player* livePlayer) {
    if (liveGame == NULL || savedWeapon == NULL || livePlayer == NULL) return;   // || means OR: any of the three missing, nothing to unpack

    livePlayer->weapon.equipped = getCardById(liveGame, savedWeapon->equippedWeaponId);
    livePlayer->weapon.killCount = savedWeapon->weaponKillCount;

    for (int stackIndex = 0; stackIndex < MAX_MONSTER_WEAPON_STACK; stackIndex++) {
        int targetId = savedWeapon->killedMonsterStackIds[stackIndex];

        livePlayer->weapon.monsterStack[stackIndex] = getCardById(liveGame, targetId);
    }
}

void unpackPile(Game* liveGame, PilesSaveData* savedPile, Zone* livePile) {
    if (liveGame == NULL || savedPile == NULL || livePile == NULL) return;   // || means OR: any of the three missing, nothing to unpack

    for (int slotIndex = 0; slotIndex < DECK_SIZE; slotIndex++) {
        livePile->cards[slotIndex] = NULL;
    }

    livePile->count = savedPile->pileCount;
    livePile->topIndex = savedPile->pileTopIndex;

    for (int cardSlot = 0; cardSlot < savedPile->pileCount; cardSlot++) {
        int targetId = savedPile->pileIds[cardSlot];

        livePile->cards[cardSlot] = getCardById(liveGame, targetId);
    }
}

void unpackRoomSlots(Game* liveGame, int* savedRoomSlotIds, Card** liveRoomSlots) {
    if (liveGame == NULL || savedRoomSlotIds == NULL || liveRoomSlots == NULL) return;   // || means OR: any of the three missing, nothing to unpack

    for (int roomSlot = 0; roomSlot < MAX_ROOM_SIZE; roomSlot++) {
        int targetId = savedRoomSlotIds[roomSlot];

        liveRoomSlots[roomSlot] = getCardById(liveGame, targetId);
    }
}

void unpackGameSaveData(GameSaveData* saveData, Game* liveGame) {
    if (saveData == NULL || liveGame == NULL) return;   // || means OR: nothing saved, or no live game to fill

    unpackPlayer(&saveData->player, &liveGame->playerOne);
    unpackWeapon(liveGame, &saveData->weapon, &liveGame->playerOne);
    unpackPile(liveGame, &saveData->mainDeck, &liveGame->mainDeck);
    unpackPile(liveGame, &saveData->discardPile, &liveGame->discardPile);
    unpackRoomSlots(liveGame, saveData->roomSlotIds, liveGame->roomSlots);

    liveGame->lastResolvedCard = getCardById(liveGame, saveData->lastResolvedCardId);
}

// ========================================================
// File Format Helpers
// ========================================================

// Reads the next word and checks it is the label that belongs here.
bool readLabel(FILE* file, const char* expectedLabel) {
    char foundLabel[SAVE_LABEL_SIZE];

    if (fscanf(file, " %31s", foundLabel) != 1) return false;   // 31 must stay one less than SAVE_LABEL_SIZE
    if (strcmp(foundLabel, expectedLabel) != 0) return false;   // strcmp says 0 when two pieces of text match

    return true;
}

// Writes one "Label: 42" line.
void writeLabelledInt(FILE* file, const char* label, int value) {
    fprintf(file, "%s %d\n", label, value);
}

// Reads one "Label: 42" line, and says false if the label or the number isn't there.
bool readLabelledInt(FILE* file, const char* label, int* valueOut) {
    if (!readLabel(file, label)) return false;              // If the line didn't start with the label we wanted
    if (fscanf(file, " %d", valueOut) != 1) return false;   // fscanf says how many values it filled in

    return true;
}

// Writes one "Label: 1 2 3" line.
void writeLabelledList(FILE* file, const char* label, const int* values, int total) {
    fprintf(file, "%s ", label);

    for (int index = 0; index < total; index++) {
        fprintf(file, "%d ", values[index]);
    }

    fprintf(file, "\n");
}

// Reads one "Label: 1 2 3" line. Every number has to be there.
bool readLabelledList(FILE* file, const char* label, int* values, int total) {
    if (!readLabel(file, label)) return false;   // If the line didn't start with the label we wanted

    for (int index = 0; index < total; index++) {
        if (fscanf(file, " %d", &values[index]) != 1) return false;   // If a number was missing from the list
    }

    return true;
}

// ========================================================
// Save Helpers
// ========================================================
void savePlayer(FILE* file, PlayerSaveData* player) {
    if (file == NULL || player == NULL) return;   // || means OR: no file, or nothing to write into it

    writeLabelledInt(file, SAVE_LABEL_PLAYER_HEALTH, player->playerHealth);
    writeLabelledInt(file, SAVE_LABEL_CAN_FLEE, player->canFlee);
    writeLabelledInt(file, SAVE_LABEL_POTION_USED, player->potionUsedThisTurn);
}

void saveWeapon(FILE* file, WeaponSaveData* weapon) {
    if (file == NULL || weapon == NULL) return;   // || means OR: no file, or nothing to write into it

    writeLabelledInt(file, SAVE_LABEL_WEAPON_ID, weapon->equippedWeaponId);
    writeLabelledInt(file, SAVE_LABEL_WEAPON_KILL_COUNT, weapon->weaponKillCount);
    writeLabelledList(file, SAVE_LABEL_MONSTER_STACK, weapon->killedMonsterStackIds, MAX_MONSTER_WEAPON_STACK);
}

void savePile(FILE* file, const char* countLabel, const char* topIndexLabel, const char* idsLabel, PilesSaveData* pile) {
    if (file == NULL || pile == NULL) return;   // || means OR: no file, or nothing to write into it

    writeLabelledInt(file, countLabel, pile->pileCount);
    writeLabelledInt(file, topIndexLabel, pile->pileTopIndex);
    writeLabelledList(file, idsLabel, pile->pileIds, pile->pileCount);
}

// ========================================================
// Load Helpers
// ========================================================
// A count or top index outside the ring would overrun pileIds, so a save carrying one is unusable.
bool isPileSizeValid(PilesSaveData* pile) {
    if (pile->pileCount < 0 || pile->pileCount > DECK_SIZE) return false;         // || means OR: a count below zero, or more cards than the ring holds
    if (pile->pileTopIndex < 0 || pile->pileTopIndex >= DECK_SIZE) return false;   // || means OR: a top index below zero, or past the last slot

    return true;
}

bool loadPlayer(FILE* file, PlayerSaveData* player) {
    if (file == NULL || player == NULL) return false;   // || means OR: no file, or nowhere to put what we read

    int canFleeFlag = 0;
    int potionUsedFlag = 0;
    bool loadOK = true;   // &= keeps this true only while every read below works

    loadOK &= readLabelledInt(file, SAVE_LABEL_PLAYER_HEALTH, &player->playerHealth);
    loadOK &= readLabelledInt(file, SAVE_LABEL_CAN_FLEE, &canFleeFlag);
    loadOK &= readLabelledInt(file, SAVE_LABEL_POTION_USED, &potionUsedFlag);

    player->canFlee = (canFleeFlag != 0);   // any value other than 0 reads as true
    player->potionUsedThisTurn = (potionUsedFlag != 0);

    return loadOK;
}

bool loadWeapon(FILE* file, WeaponSaveData* weapon) {
    if (file == NULL || weapon == NULL) return false;   // || means OR: no file, or nowhere to put what we read

    bool loadOK = true;   // &= keeps this true only while every read below works

    loadOK &= readLabelledInt(file, SAVE_LABEL_WEAPON_ID, &weapon->equippedWeaponId);
    loadOK &= readLabelledInt(file, SAVE_LABEL_WEAPON_KILL_COUNT, &weapon->weaponKillCount);
    loadOK &= readLabelledList(file, SAVE_LABEL_MONSTER_STACK, weapon->killedMonsterStackIds, MAX_MONSTER_WEAPON_STACK);

    return loadOK;
}

bool loadPile(FILE* file, const char* countLabel, const char* topIndexLabel, const char* idsLabel, PilesSaveData* pile) {
    if (file == NULL || pile == NULL) return false;   // || means OR: no file, or nowhere to put what we read

    bool loadOK = true;   // &= keeps this true only while every read below works

    loadOK &= readLabelledInt(file, countLabel, &pile->pileCount);
    loadOK &= readLabelledInt(file, topIndexLabel, &pile->pileTopIndex);

    // The count sets how many IDs to read below, so both it and the file have to be sound first.
    if (!loadOK) return false;                   // If either line above failed to load
    if (!isPileSizeValid(pile)) return false;    // If the saved count or top index won't fit the ring

    loadOK &= readLabelledList(file, idsLabel, pile->pileIds, pile->pileCount);

    return loadOK;
}

// ========================================================
// Core Save & Load Managers
// ========================================================
bool saveGameData(const char* filepath, GameSaveData* data) {
    if (filepath == NULL || data == NULL) return false;   // || means OR: no path to write to, or nothing to save

    FILE* file = fopen(filepath, "w");
    if (file == NULL) return false;

    savePlayer(file, &data->player);
    saveWeapon(file, &data->weapon);
    savePile(file, SAVE_LABEL_MAIN_DECK_COUNT, SAVE_LABEL_MAIN_DECK_TOP_INDEX, SAVE_LABEL_MAIN_DECK_IDS, &data->mainDeck);
    savePile(file, SAVE_LABEL_DISCARD_COUNT, SAVE_LABEL_DISCARD_TOP_INDEX, SAVE_LABEL_DISCARD_IDS, &data->discardPile);
    writeLabelledInt(file, SAVE_LABEL_LAST_RESOLVED, data->lastResolvedCardId);
    writeLabelledList(file, SAVE_LABEL_ROOM_SLOTS, data->roomSlotIds, MAX_ROOM_SIZE);

    bool saveOK = (ferror(file) == 0);   // ferror flags a write that failed anywhere along the way

    if (fclose(file) != 0) return false;       // the last of the file may only reach the disk on close

    return saveOK;
}

// Reads a save file's body. The whole file format, in the order it sits on disk.
bool readGameSaveData(FILE* file, GameSaveData* data) {
    bool loadOK = true;   // &= keeps this true only while every read below works

    loadOK &= loadPlayer(file, &data->player);
    loadOK &= loadWeapon(file, &data->weapon);
    loadOK &= loadPile(file, SAVE_LABEL_MAIN_DECK_COUNT, SAVE_LABEL_MAIN_DECK_TOP_INDEX, SAVE_LABEL_MAIN_DECK_IDS, &data->mainDeck);
    loadOK &= loadPile(file, SAVE_LABEL_DISCARD_COUNT, SAVE_LABEL_DISCARD_TOP_INDEX, SAVE_LABEL_DISCARD_IDS, &data->discardPile);
    loadOK &= readLabelledInt(file, SAVE_LABEL_LAST_RESOLVED, &data->lastResolvedCardId);
    loadOK &= readLabelledList(file, SAVE_LABEL_ROOM_SLOTS, data->roomSlotIds, MAX_ROOM_SIZE);

    return loadOK;
}

bool loadGameData(const char* filepath, GameSaveData* data) {
    if (filepath == NULL || data == NULL) return false;   // || means OR: no path to read from, or nowhere to load into

    FILE* file = fopen(filepath, "r");
    if (file == NULL) return false;

    bool loadOK = readGameSaveData(file, data);

    fclose(file);

    return loadOK;
}
