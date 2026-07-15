#include "UI_manager.h"
#include "Data_Structure.h"
#include "Game_master.h"
#include <iso646.h>
#include <stdio.h>

// ========================================================
// Basic utilities
// ========================================================
void clearScreen() {
    printf("\033[H\033[J"); 
}

// ========================================================
// Debug printing (targeted)
// ========================================================
void printCurrentPlayerStats(struct game* activeSession) {
    struct player* p1 = &activeSession->playerOne;
    struct card* equippedWeapon = p1->weapon.equipped;
    
    int currentWeaponValue = 0;
    
    if (equippedWeapon != NULL) {
        currentWeaponValue = equippedWeapon->value;
    }

    printf("Max HP: %d\n", p1->maxHP);
    printf("Current HP: %d\n", p1->hp);
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
            cardData->id, 
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
    if (!isGameSessionActive(activeSession)) {
        printf("ERROR: No active game session is currently running!\n");
        return;
    }

    Zone* deck = &activeSession->mainDeck;
    printEntireDeckLoop(deck);
}

void printDungeonRoom(struct game* activeSession) {
    if (!isGameSessionActive(activeSession)) {
        printf("ERROR: No active game session is currently running!\n");
        return;
    }

    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        CardLink* slotLink = (CardLink*)activeSession->roomSlots[i];
        
        if (slotLink == NULL) {
            printf("Dungeon Slot %d | [EMPTY]\n", i + 1);
        } else {
            struct card* cardData = slotLink->data;
            printf("Dungeon Slot %d | Card ID: %d | Type: %c | Value: %d\n",
                i + 1,
                cardData->id,
                cardData->type,
                cardData->value);
        }
    }
    
    printf("--- END OF ROOM ---\n");
}

// ========================================================
// Master debug renderer
// ========================================================
void debugRender(struct game* game) {
    struct player* p1 = &game->playerOne;
    struct weapon* wpn = &p1->weapon;
    Zone* deck = &game->mainDeck;
    
    printf("\n=== SCOUNDREL DEBUG VIEW ===\n");
    
    printf("HP: %d/%d\n", p1->hp, p1->maxHP);
    
    if (wpn->equipped == NULL) {
        printf("Weapon: [UNARMED]\n");
    } else {
        int weaponValue = wpn->equipped->value;
        int kills = wpn->killCount;
        
        if (kills == 0) {
            printf("Weapon Value: %d | Last Kill: [NONE]\n", weaponValue);
        } else {
            int lastKillIndex = kills - 1;
            struct card* lastKilledMonster = wpn->monsterStack[lastKillIndex];
            
            printf("Weapon Value: %d | Last Kill: %d\n", weaponValue, lastKilledMonster->value);
        }
    }

    printf("Cards in Deck: %d\n", deck->count);
    
    if (p1->canFlee) {
        printf("Can Flee Room? YES\n");
    } else {
        printf("Can Flee Room? NO\n");
    }
    
    printf("----------------------------\n");
    
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        CardLink* slotLink = (CardLink*)game->roomSlots[i];
        
        if (slotLink == NULL) {
            printf("Slot %d: [EMPTY]\n", i + 1);
        } else {
            struct card* slotCard = slotLink->data;
            printf("Slot %d: [%d of %c]\n", i + 1, slotCard->value, slotCard->type);
        }
    }
    printf("============================\n");
}