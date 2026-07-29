#include "Input.h"
#include "Data_Structure.h"
#include "UI_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ========================================================
// Input processing
// ========================================================
int processUserInput(void) {
    char rawUserInput[21];

    if (scanf("%20s", rawUserInput) != 1) {
        return INPUT_END_OF_STREAM;
    }

    if (strcmp(rawUserInput, "debug") == 0) {
        return INPUT_DEBUG_COMMAND;
    }

    if (isdigit((unsigned char)rawUserInput[0])) {
        return atoi(rawUserInput);
    }

    renderUnknownCommand();

    return INPUT_INVALID;
}

// ========================================================
// Utilities
// ========================================================
void pressEnterToContinue(void) {
    int flushedCharacter;

    while ((flushedCharacter = getchar()) != '\n' && flushedCharacter != EOF) { }

    printf("\nPress [Enter] to continue...");

    getchar();
}
