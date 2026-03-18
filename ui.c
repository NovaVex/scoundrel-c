#include "mainHeader.h"

int drawMainMenu() {
    int userInput;

printf(
    " ================================= \n"
    " |                               | \n"
    " |          Scoundrel            | \n"
    " |                               | \n"
    " ================================= \n"
    "                                   \n"
    "        [1] Start Game             \n"
    "        [2] Options                \n"
    "        [3] Quit                   \n"
    "                                   \n"
    "Choose an option: "
);

userInputExstraction();

return userInput;

}

int drawDebugMenu(){
    int userInput;

printf(
    " ================================= \n"
    " |                               | \n"
    " |          Debug Menu           | \n"
    " |                               | \n"
    " ================================= \n"
    "                                   \n"
    "        [1] Print Cards            \n"
    "        [2] Options                \n"
    "        [3] Quit                   \n"
    "                                   \n"
    "Choose an option: ")

userInputExstraction();

}