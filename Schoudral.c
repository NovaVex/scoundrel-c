#include <stdio.h>

struct _card {
    int _id;
    char *_type;
    int _cardValue;
};

int main() {
    int cardValueGenerator = 2;
    int deckIncrmenter;
    int usrInput;

    struct _card mainDeck[44];

    // Chunk 1: Monsters
    for (deckIncrmenter = 0; deckIncrmenter < 26; deckIncrmenter++) {
        if(cardValueGenerator  == 15 ){
            cardValueGenerator = 2;
}
            mainDeck[deckIncrmenter]._id = deckIncrmenter + 1;
            mainDeck[deckIncrmenter]._type = "Monsters";
            mainDeck[deckIncrmenter]._cardValue = cardValueGenerator++;
}

    cardValueGenerator = 2;

    // Chunk 2: Potions
    for (deckIncrmenter = 26; deckIncrmenter < 35; deckIncrmenter++) {
        mainDeck[deckIncrmenter]._id = deckIncrmenter + 1; 
        mainDeck[deckIncrmenter]._type = "Potion";
        mainDeck[deckIncrmenter]._cardValue = cardValueGenerator++;
    }

    cardValueGenerator = 2;

    // Chunk 3: Weapons
    for (deckIncrmenter = 35; deckIncrmenter < 44; deckIncrmenter++) {
        mainDeck[deckIncrmenter]._id = deckIncrmenter + 1; 
        mainDeck[deckIncrmenter]._type = "Weapon";
        mainDeck[deckIncrmenter]._cardValue = cardValueGenerator++;
    }
printf(
    " ================================= \n"
    " |                               | \n"
    " |          Scoundrel            | \n"
    " |                               | \n"
    " ================================= \n"
    "                                   \n"
    "        [1] Start Game             \n"
    "        [2] View Deck              \n"
    "        [3] Quit                   \n"
    "                                   \n"
    "Choose an option: "
);

scanf("%d", &usrInput);
    if(usrInput == 2) {
        for (int i = 0; i < 44; i++) {
    // Print each piece of data from the current card
    printf("Card ID: %d | Type: %s | Value: %d\n", 
           mainDeck[i]._id, 
           mainDeck[i]._type, 
           mainDeck[i]._cardValue);
}
}
return 0;
}
