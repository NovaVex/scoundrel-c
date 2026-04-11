#include "scene_manager.h"
#include "input.h"
#include <stdio.h>


int openMainMenu(){

	clearScreen();
	drawMainMenu();
	int userInput = processUserInput();

	return userInput;

}


