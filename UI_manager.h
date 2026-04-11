#pragma once

void pressEnterToContinue(){
    int c;
    
    while ((c = getchar()) != '\n' && c != EOF) { } 
    
    printf("\nPress [Enter] to continue...");
    
    getchar(); 
}

void clearScreen(){

    printf("\033[H\033[J"); 
}