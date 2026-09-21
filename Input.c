// ========================================================
// Input.c
// --------------------------------------------------------
// Keyboard reading. Throwaway at the engine port.
// ========================================================

#include "Input.h"
#include "Terminal_Input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ========================================================
// Input processing
// ========================================================
int processUserInput(void) {
    char rawUserInput[INPUT_BUFFER_SIZE];

    if (scanf("%20s", rawUserInput) != 1) return INPUT_END_OF_STREAM;   // 20 must stay one less than INPUT_BUFFER_SIZE
    if (strcmp(rawUserInput, DEBUG_COMMAND_WORD) == 0) return INPUT_DEBUG_COMMAND;
    if (!isdigit((unsigned char)rawUserInput[0])) return INPUT_INVALID;   // If it doesn't start with a number

    int typedNumber = atoi(rawUserInput);

    return typedNumber;
}

// ========================================================
// Utilities
// ========================================================
void pressEnterToContinue(void) {
    int flushedCharacter;

    // && means AND: keep reading until the end of the line, or the end of input
    while ((flushedCharacter = getchar()) != '\n' && flushedCharacter != EOF) { }

    printf("\nPress [Enter] to continue...");

    getchar();
}
