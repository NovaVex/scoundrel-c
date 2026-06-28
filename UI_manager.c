#include "UI_manager.h"
#include "Data_Structure.h"
#include <stdio.h>

// ========================================================
// Local word helpers
// ========================================================
static const char* yesOrNo(bool state) {
    if (state) return "YES";
    return "NO";
}

static const char* cardTypeName(char type) {
    if (type == MONSTER) return "Monster";
    if (type == POTION) return "Potion";
    if (type == WEAPON) return "Weapon";
    return "???";
}

// ========================================================
// Basic utilities
// ========================================================
void clearScreen() {
    printf("\033[H\033[J");
}

// ========================================================
// Player-facing rendering
// ========================================================
void renderGameView(struct game* session) {
    printf("==============================\n");
    printf("          SCOUNDREL           \n");
    printf("==============================\n");

    printHealthLine(&session->playerOne, session->mainDeck.count);
    printWeaponLine(&session->playerOne.weapon);

    printf("------------------------------\n");
    printf("THE ROOM:\n");
    printRoomSlots(session);
    printf("==============================\n");
}

void printHealthLine(struct player* activePlayer, int cardsInDeck) {
    printf("HP: %d/%d   |   Dungeon: %d cards\n",
        activePlayer->currentHealth,
        activePlayer->maximumHealth,
        cardsInDeck);
}

void printWeaponLine(struct weapon* playerWeapon) {
    if (playerWeapon->equippedCard == NULL) {
        printf("Weapon: [UNARMED]\n");
        return;
    }

    if (playerWeapon->killCount == 0) {
        printf("Weapon: %d (fresh)\n", playerWeapon->equippedCard->value);
        return;
    }

    int lastKillIndex = playerWeapon->killCount - 1;
    int lastKillValue = playerWeapon->monsterStack[lastKillIndex]->value;

    printf("Weapon: %d (can only fight monsters of %d or weaker)\n",
        playerWeapon->equippedCard->value,
        lastKillValue);
}

void printRoomSlots(struct game* session) {
    for (int slotIndex = 0; slotIndex < MAX_ROOM_SIZE; slotIndex++) {
        CardLink* slotLink = session->roomSlots[slotIndex];

        if (slotLink == NULL) {
            printf("  Slot %d: [EMPTY]\n", slotIndex + 1);
            continue;
        }

        printf("  Slot %d: %s (%d)\n",
            slotIndex + 1,
            cardTypeName(slotLink->data->type),
            slotLink->data->value);
    }
}

// ========================================================
// Player-facing messages
// ========================================================
void printEncounterMessage(EncounterResult result) {
    if (result == ENCOUNTER_POTION_FIZZLED) {
        printf("\nThe second potion this turn fizzles away... (no effect)\n");
        return;
    }

}

void printLastCardWarning() {
    printf("\nThe last card belongs to the next room. Move on! (8)\n");
}

void printGameOverScreen(int finalScore) {
    printf("\n\n=== GAME OVER ===\n");
    printf("You have fallen in the dungeon.\n");
    printf("Final Score: %d\n", finalScore);
}

void printVictoryScreen(int finalScore) {
    printf("\n\n=== VICTORY ===\n");
    printf("You cleared the entire dungeon!\n");
    printf("Final Score: %d\n", finalScore);
}

// ========================================================
// Debug printing (targeted)
// ========================================================
void printCurrentPlayerStats(struct game* activeSession) {
    struct player* activePlayer = &activeSession->playerOne;

    int currentWeaponValue = 0;
    if (activePlayer->weapon.equippedCard != NULL) {
        currentWeaponValue = activePlayer->weapon.equippedCard->value;
    }

    printf("Max HP: %d\n", activePlayer->maximumHealth);
    printf("Current HP: %d\n", activePlayer->currentHealth);
    printf("Weapon Value: %d\n", currentWeaponValue);
}

void printEntireDeckLoop(Zone* pile) {
    CardLink* currentLink = pile->topCard;
    int position = 1;

    if (currentLink == NULL) {
        printf("--- PILE IS EMPTY ---\n");
        return;
    }

    while (currentLink != NULL) {
        struct card* cardData = currentLink->data;

        printf("Position: %d | Card ID: %d | Type: %c | Value: %d\n",
            position,
            cardData->identifier,
            cardData->type,
            cardData->value);

        currentLink = currentLink->next;
        position++;
    }

    printf("--- END OF PILE ---\n");
}

// ========================================================
// Debug tools (session level)
// ========================================================
void printSessionDeck(struct game* activeSession) {
    if (activeSession == NULL) {
        printf("ERROR: No active game session is currently running!\n");
        return;
    }

    printEntireDeckLoop(&activeSession->mainDeck);
}

void printDungeonRoom(struct game* activeSession) {
    if (activeSession == NULL) {
        printf("ERROR: No active game session is currently running!\n");
        return;
    }

    for (int slotIndex = 0; slotIndex < MAX_ROOM_SIZE; slotIndex++) {
        CardLink* slotLink = activeSession->roomSlots[slotIndex];

        if (slotLink == NULL) {
            printf("Dungeon Slot %d | [EMPTY]\n", slotIndex + 1);
            continue;
        }

        struct card* cardData = slotLink->data;
        printf("Dungeon Slot %d | Card ID: %d | Type: %c | Value: %d\n",
            slotIndex + 1,
            cardData->identifier,
            cardData->type,
            cardData->value);
    }

    printf("--- END OF ROOM ---\n");
}

// ========================================================
// Master debug renderer
// ========================================================
void debugRender(struct game* session) {
    struct player* activePlayer = &session->playerOne;
    struct weapon* playerWeapon = &activePlayer->weapon;

    printf("\n=== SCOUNDREL DEBUG VIEW ===\n");
    printf("HP: %d/%d\n", activePlayer->currentHealth, activePlayer->maximumHealth);

    printWeaponLine(playerWeapon);

    printf("Cards in Deck: %d\n", session->mainDeck.count);
    printf("Potion used this room? %s\n", yesOrNo(activePlayer->potionUsedThisRoom));
    printf("Can Flee Room? %s\n", yesOrNo(activePlayer->canFlee));
    printf("----------------------------\n");

    printRoomSlots(session);
    printf("============================\n");
}
