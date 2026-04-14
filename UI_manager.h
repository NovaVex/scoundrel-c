#pragma once

void clearScreen();

//Debug Functions
void printDungeonRoom(struct game* activeSession);
void printSessionDeck(struct game* activeSession);
void printEntireDeckLoop(struct deck* deck );
void printCurrentPlayerStats(struct game* activeSession);
void printEntireDungeonRoom(struct dungeon* dungeon);