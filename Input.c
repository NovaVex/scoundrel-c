#include "Input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ========================================================
// Input processing
// ========================================================
int processUserInput() {
    char rawUserInput[21];

    if (scanf("%20s", rawUserInput) != 1) {
        if (feof(stdin)) exit(0);
        return -1;
    }

    if (isalpha(rawUserInput[0])) {
        if (strcmp(rawUserInput, "debug") == 0) {
            printf("Debug command recognized.\n");
            return 99;
        }
        printf("Unknown command. Please use 0-9 to make a selection.\n");
        return -1;
    }

    if (isdigit(rawUserInput[0])) {
        int playerChoice = atoi(rawUserInput);
        return playerChoice;
    }

    printf("Unknown command. Please use 0-9 to make a selection.\n");
    return -1;
}

// ========================================================
// Utilities
// ========================================================
void pressEnterToContinue() {
    int leftoverCharacter;

    while ((leftoverCharacter = getchar()) != '\n' && leftoverCharacter != EOF) { }

    printf("\nPress [Enter] to continue...");
    getchar();
}
