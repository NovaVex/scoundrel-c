#pragma once

//Input Definitions
#define INPUT_BUFFER_SIZE 21   // 20 characters, plus 1 for the \0 that ends the text
#define DEBUG_COMMAND_WORD "debug"

// ========================================================
// Input processing
// ========================================================
int processUserInput(void);

// ========================================================
// Utilities
// ========================================================
void pressEnterToContinue(void);
